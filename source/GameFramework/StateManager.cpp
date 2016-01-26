#include "GameManager.h"
#include "StateManager.h"

#include <map>
#include <algorithm>

std::string STATE_ENTRY_EVENT = "StateEntry";
std::string STATE_CHANGE_EVENT = "StateChange";
std::string STATE_EXIT_EVENT = "StateExit";

bool traceState = false;

EventCallback::EventCallback(std::string eventName, IGameObject *eventObject, EventCallbackPtr eventMethod, int32 priority)
{
    this->eventName = eventName;
    this->gameTime = 0;
    this->eventObject = eventObject;
    this->eventMethod = eventMethod;
    this->eventData = null;
    this->subscriberData = null;
    this->priority = priority;
}

void EventCallback::Dispatch(uint32 gameTime, void *eventData)
{
    if(eventObject == null || eventMethod == null)
    {
        return;
    }
    this->gameTime = gameTime;
    this->eventData = eventData;

    CALL_MEMBER_FN(eventObject, eventMethod)(this);
}

EventSubscriber::EventSubscriber(std::string eventName, StateManager *state)
{
    this->state = state;
    this->eventName = eventName;
    callbacks = new std::vector<EventCallback *>;
}

EventSubscriber::~EventSubscriber()
{
    for(uint32 index = 0; index < callbacks->size(); index++)
    {
        delete callbacks->at(index);
    }

    delete callbacks;
}

void EventSubscriber::AddCallback(EventCallback *callback)
{
    callbacks->push_back(callback);
    needsSort = true;
}

bool compareEventPriority(EventCallback *first, EventCallback *second)
{
    if(first == null && second == null) return false;
    if(first == null) return true;
    if(second == null) return false;

    int firstPriority = first->priority;
    int secondPriority = second->priority;

    return firstPriority < secondPriority;
}

void EventSubscriber::Dispatch(uint32 gameTime, void *eventData)
{
    if(needsSort)
    {
        std::sort(callbacks->begin(), callbacks->end(), compareEventPriority);
        needsSort = false;
    }

    for(uint32 index = 0; index < callbacks->size(); index++)
    {
        EventCallback *callback = callbacks->at(index);
        if(traceState) s3eDebugTracePrintf("Dispatching event %s to object %s at time %d", this->eventName.c_str(), callback->eventObject->debugId, gameTime);
        callback->Dispatch(gameTime, eventData);
    }
}

void EventSubscriber::NotifyGameObjectDeletion(IGameObject *gameObject)
{
    for(int32 index = (int32)callbacks->size() - 1; index >= 0; index--)
    {
        EventCallback *callback = callbacks->at(index);
        if(callback->eventObject == gameObject)
        {
            callbacks->erase(callbacks->begin()+index);
            delete callback;
        }
    }
}


const std::string StateManager::INVALID_STATE = "**InvalidState**";
const std::string StateManager::INVALID_VARIABLE = "**InvalidVariable**";
StateVariable StateManager::zeroVar;

StateManager::StateManager(GameManager *mgr)
{
    this->mgr = mgr;
    varNames = new std::vector<std::string>;
    vars = new std::vector<StateVariable>;
    eventSubscribers = new std::map<std::string, EventSubscriber*>;
    futureEventList = new std::vector<EventCallback *>;
    postClearFutureEventList = new std::vector<EventCallback *>;
    felNeedsSort = false;
    clearFelPending = false;

    validStates = new std::vector<std::string>;

    anyEntrySubscriber = new EventSubscriber(STATE_ENTRY_EVENT, this);
    anyTransitionSubscriber = new EventSubscriber(STATE_CHANGE_EVENT, this);
    anyExitSubscriber = new EventSubscriber(STATE_EXIT_EVENT, this);

    stateEntrySubscribers = new std::vector<EventSubscriber *>;
    stateTransitionSubscribers = new std::vector<std::vector<EventSubscriber*> *>;
    stateExitSubscribers = new std::vector<EventSubscriber *>;
    currentState = 0;

    zeroVar.intValue = 0;
    RegisterGameState(INVALID_STATE);
    RegisterVariable(INVALID_VARIABLE, zeroVar);
}

StateManager::~StateManager()
{
    delete vars;
    delete varNames;

    delete anyEntrySubscriber;
    delete anyTransitionSubscriber;
    delete anyExitSubscriber;

    for(uint32 subIndex = 0; subIndex < stateEntrySubscribers->size(); subIndex++)
    {
        delete stateEntrySubscribers->at(subIndex);
    }

    for(uint32 index = 0; index < stateTransitionSubscribers->size(); index++)
    {
        for(uint32 subIndex = 0; subIndex < stateTransitionSubscribers->at(index)->size(); subIndex++)
        {
            delete stateTransitionSubscribers->at(index)->at(subIndex);
        }

        delete stateTransitionSubscribers->at(index);
    }

    for(uint32 subIndex = 0; subIndex < stateExitSubscribers->size(); subIndex++)
    {
        delete stateExitSubscribers->at(subIndex);
    }

    delete stateEntrySubscribers;
    delete stateTransitionSubscribers;
    delete stateExitSubscribers;

    delete validStates;

    for(uint32 index = 0; index < futureEventList->size(); index++)
    {
        delete futureEventList->at(index);
    }

    delete futureEventList;

    for(uint32 index = 0; index < postClearFutureEventList->size(); index++)
    {
        delete postClearFutureEventList->at(index);
    }

    delete postClearFutureEventList;

    std::map<std::string, EventSubscriber *>::iterator it;
    for ( it=eventSubscribers->begin() ; it != eventSubscribers->end(); it++ )
    {
        delete (*it).second;
    }

    delete eventSubscribers;
}

GameManager *StateManager::GetGameManager()
{
    return mgr;
}

uint32 StateManager::RegisterVariable(std::string varName, StateVariable initialValue)
{
    uint32 returnHandle = vars->size();

    vars->push_back(initialValue);
    varNames->push_back(varName);

    return returnHandle;
}

StateVariable StateManager::GetVar(uint32 varHandle)
{
    if(varHandle == 0 || varHandle >= vars->size())
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Invalid variable handle passed to GetVar");
        return zeroVar;
    }

    return vars->at(varHandle);
}

void StateManager::SetVar(uint32 varHandle, StateVariable value)
{
    if(varHandle == 0 || varHandle >= vars->size())
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Invalid variable handle passed to SetVar");
        return;
    }

    StateVariable oldValue = vars->at(varHandle);
    vars->at(varHandle) = value;

    if(oldValue.uintValue != value.uintValue)
    {
        VarEventData *eventData = new VarEventData();

        eventData->varName = varNames->at(varHandle);
        eventData->varHandle = varHandle;
        eventData->oldValue = oldValue;
        eventData->newValue = value;

        DispatchVarChangeEvent(varHandle, eventData);
        delete eventData;
    }
}

std::string VarChangeEventName(std::string varName)
{
    std::string eventName = "**VarChangeEvent**";
    eventName += varName;
    return eventName;
}

void StateManager::SubscribeStateVarChange(uint32 varHandle, IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData, int32 priority)
{
    if(varHandle == 0 || varHandle >= vars->size())
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Invalid variable handle passed to SubscribeStateVarChange");
        return;
    }

    std::string eventName = VarChangeEventName(varNames->at(varHandle));
    EventSubscriber *subscriber = (*eventSubscribers)[eventName];

    if(subscriber == null)
    {
        subscriber = new EventSubscriber(eventName, this);
        (*eventSubscribers)[eventName] = subscriber;
    }

    EventCallback *eventCallback = new EventCallback(varNames->at(varHandle), eventObject, eventMethod, priority);
    eventCallback->subscriberData = subscriberData;
    subscriber->AddCallback(eventCallback);
    if(traceState) s3eDebugTracePrintf("Registering subscription for event %s from object %s", eventName.c_str(), eventObject->debugId);
}

void StateManager::DispatchVarChangeEvent(uint32 varHandle, VarEventData *eventData)
{
    EventSubscriber *subscriber = (*eventSubscribers)[VarChangeEventName(varNames->at(varHandle))];

    if(subscriber != null)
    {
        subscriber->Dispatch(mgr->GetCurrentClockTime(), eventData);
    }
}

void StateManager::PostEvent(std::string eventName, void *eventData, uint32 gameTime)
{
    if(gameTime == 0)
    {
        gameTime = mgr->GetCurrentClockTime();
    }

    EventSubscriber *subscriber = (*eventSubscribers)[eventName];

    if(traceState) s3eDebugTracePrintf("Posting event %s at time %d", eventName.c_str(), gameTime);

    if(subscriber != null)
    {
        subscriber->Dispatch(gameTime, eventData);
    }
}

void StateManager::SubscribeEvent(std::string eventName, IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData, int32 priority)
{
    EventSubscriber *subscriber = (*eventSubscribers)[eventName];

    if(subscriber == null)
    {
        subscriber = new EventSubscriber(eventName, this);
        (*eventSubscribers)[eventName] = subscriber;
    }

    EventCallback *eventCallback = new EventCallback(eventName, eventObject, eventMethod, priority);
    eventCallback->subscriberData = subscriberData;
    subscriber->AddCallback(eventCallback);
    if(traceState) s3eDebugTracePrintf("Registering subscription for event %s from object %s", eventName.c_str(), eventObject->debugId);
}

bool FELSorter(EventCallback *first, EventCallback *second)
{
    if(first == null && second == null) return false;
    if(first == null) return true;
    if(second == null) return false;

    if(first->gameTime < second->gameTime)
    {
        return true;
    }

    if(first->gameTime == second->gameTime)
    {
        return first->priority < second->priority;
    }

    return false;
}

void StateManager::PostFutureEvent(std::string eventName, uint32 eventTimeMillis, void *eventData, int32 priority)
{
    EventCallback *eventCallback = new EventCallback(eventName, null, null, priority);
    eventCallback->gameTime = eventTimeMillis;
    eventCallback->eventData = eventData;

    if(clearFelPending)
    {
        postClearFutureEventList->push_back(eventCallback);
    }
    else
    {
        futureEventList->push_back(eventCallback);
    }

    felNeedsSort = true;
    if(traceState) s3eDebugTracePrintf("Posting future event %s to fire at time %d, time now: %d", eventName.c_str(), eventTimeMillis, mgr->GetCurrentClockTime());
}

void StateManager::Update(uint32 gameTime)
{
    uint32 index;

    if(clearFelPending)
    {
        for(index = 0; index < futureEventList->size(); index++)
        {
            EventCallback *checkEvent = futureEventList->at(index);
            delete checkEvent;
        }

        futureEventList->clear();

        if(traceState) s3eDebugTracePrintf("Cleared FEL, time now: %d, adding %d post-clear events", mgr->GetCurrentClockTime(), postClearFutureEventList->size());

        for(index = 0; index < postClearFutureEventList->size(); index++)
        {
            EventCallback *transferEvent = postClearFutureEventList->at(index);
            futureEventList->push_back(transferEvent);
        }

        postClearFutureEventList->clear();

        if(futureEventList->size() > 0)
        {
            felNeedsSort = true;
        }
    }

    clearFelPending = false;

    if(futureEventList->size() == 0)
    {
        return;
    }

    if(felNeedsSort)
    {
        std::sort(futureEventList->begin(), futureEventList->end(), FELSorter);
        felNeedsSort = false;
    }


    for(index = 0; index < futureEventList->size(); index++)
    {
        EventCallback *checkEvent = futureEventList->at(index);

        if(checkEvent->gameTime > gameTime)
        {
            break;
        }

        checkEvent->gameTime = gameTime;
        PostEvent(checkEvent->eventName, checkEvent->eventData, gameTime);
        delete checkEvent;
    }

    if(index > 0)
    {
        futureEventList->erase(futureEventList->begin(), futureEventList->begin()+index);
    }
}

void StateManager::ClearFutureEvents()
{
    // Don't clear right now, if we are in the middle of processing the FEL then deleting the events on the FEL
    // will cause a double-delete whne the FEL deletes the event that invoked this call

    clearFelPending = true;
}

std::string StateManager::StateEntryEventName(uint32 stateHandle)
{
    std::string eventName = validStates->at(stateHandle);
    eventName += ":Entry";
    return eventName;
}

std::string StateManager::StateChangeEventName(uint32 fromState, uint32 toState)
{
    std::string eventName = validStates->at(fromState);
    eventName += ":" + validStates->at(toState);
    return eventName;
}

std::string StateManager::StateExitEventName(uint32 stateHandle)
{
    std::string eventName = validStates->at(stateHandle);
    eventName += ":Exit";
    return eventName;
}

uint32 StateManager::RegisterGameState(std::string stateName)
{
    for(uint32 stateIndex = 0; stateIndex < validStates->size(); stateIndex++)
    {
        if(validStates->at(stateIndex) == stateName)
        {
            s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Error: Duplicate state name passed to RegisterGameSate");
            return 0;
        }
    }

    int stateHandle = validStates->size();

    validStates->push_back(stateName);
    stateEntrySubscribers->push_back(new EventSubscriber(StateEntryEventName(stateHandle), this));
    stateExitSubscribers->push_back(new EventSubscriber(StateExitEventName(stateHandle), this));

    return stateHandle;
}

void StateManager::DefineValidStateTransition(uint32 fromState, uint32 toState)
{
    if(fromState == 0 || toState == 0 || fromState >= validStates->size() || toState >= validStates->size())
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Invalid state handle passed to DefineValidStateTransition");
        return;
    }

    while(stateTransitionSubscribers->size() <= fromState)
    {
        stateTransitionSubscribers->push_back(new std::vector<EventSubscriber *>);
    }

    while(stateTransitionSubscribers->at(fromState)->size() <= toState)
    {
        stateTransitionSubscribers->at(fromState)->push_back(null);
    }

    if(stateTransitionSubscribers->at(fromState)->at(toState) == null)
    {
        stateTransitionSubscribers->at(fromState)->at(toState) = new EventSubscriber(StateChangeEventName(fromState, toState), this);
    }
}

void StateManager::SetInitialState(uint32 initialState)
{
    if(initialState == 0 || initialState >= validStates->size())
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Invalid state handle passed to SetInitialState");
        return;
    }

    if(currentState != 0)
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Error, SetInitialState may only be called once");
        return;
    }

    currentState = initialState;
}

bool StateManager::ChangeState(uint32 newState)
{
    if(newState == 0 || newState >= validStates->size())
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Invalid state handle passed to ChangeState");
        return false;
    }

    EventSubscriber *changeEvent = stateTransitionSubscribers->at(currentState)->at(newState);

    if(changeEvent == null)
    {
        std::string errMsg = "Invalid state transition requested, from state ";
        errMsg += validStates->at(currentState) + " to state " + validStates->at(newState);
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, errMsg.c_str());
        return false;
    }

    std::string transitionName = StateChangeEventName(currentState, newState);

    StateChangeData *eventData = new StateChangeData;
    eventData->fromState = currentState;
    eventData->fromStateName = validStates->at(currentState);
    eventData->toState = newState;
    eventData->toStateName = validStates->at(newState);
    eventData->cancelStateChange = false;

    anyExitSubscriber->Dispatch(mgr->GetCurrentClockTime(), eventData);

    if(eventData->cancelStateChange)
    {
        delete eventData;
        return false;
    }

    EventSubscriber *exitEvent = stateExitSubscribers->at(currentState);

    if(exitEvent != null)
    {
        exitEvent->Dispatch(mgr->GetCurrentClockTime(), eventData);

        if(eventData->cancelStateChange)
        {
            delete eventData;
            return false;
        }
    }

    anyTransitionSubscriber->Dispatch(mgr->GetCurrentClockTime(), eventData);

    if(eventData->cancelStateChange)
    {
        delete eventData;
        return false;
    }

    changeEvent->Dispatch(mgr->GetCurrentClockTime(), eventData);

    if(eventData->cancelStateChange)
    {
        delete eventData;
        return false;
    }

    anyEntrySubscriber->Dispatch(mgr->GetCurrentClockTime(), eventData);

    if(eventData->cancelStateChange)
    {
        delete eventData;
        return false;
    }

    EventSubscriber *entryEvent = stateEntrySubscribers->at(newState);

    if(entryEvent != null)
    {
        entryEvent->Dispatch(mgr->GetCurrentClockTime(), eventData);

        if(eventData->cancelStateChange)
        {
            delete eventData;
            return false;
        }
    }

    delete eventData;
    currentState = newState;
    return true;
}

uint32 StateManager::GetCurrentState()
{
    return currentState;
}

std::string StateManager::GetStateName(uint32 state)
{
    if(state == 0 || state >= validStates->size())
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Invalid state handle passed to GetStateName");
        return INVALID_STATE;
    }

    return validStates->at(state);
}

void StateManager::SubscribeAnyStateEntry(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData, int32 priority)
{
    EventCallback *eventCallback = new EventCallback(STATE_ENTRY_EVENT, eventObject, eventMethod, priority);
    eventCallback->subscriberData = subscriberData;
    anyEntrySubscriber->AddCallback(eventCallback);
}

void StateManager::SubscribeAnyStateChange(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData, int32 priority)
{
    EventCallback *eventCallback = new EventCallback(STATE_CHANGE_EVENT, eventObject, eventMethod, priority);
    eventCallback->subscriberData = subscriberData;
    anyTransitionSubscriber->AddCallback(eventCallback);
}

void StateManager::SubscribeAnyStateExit(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData, int32 priority)
{
    EventCallback *eventCallback = new EventCallback(STATE_EXIT_EVENT, eventObject, eventMethod, priority);
    eventCallback->subscriberData = subscriberData;
    anyExitSubscriber->AddCallback(eventCallback);
}

void StateManager::SubscribeStateEntry(uint32 toState, IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData, int32 priority)
{
    if(toState == 0 || toState >= validStates->size())
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Invalid state handle passed to SubscribeStateEntry");
        return;
    }

    EventSubscriber *eventSubscriber = stateEntrySubscribers->at(toState);

    if(eventSubscriber == null)
    {
        eventSubscriber = new EventSubscriber(StateEntryEventName(toState), this);
        stateEntrySubscribers->at(toState) = eventSubscriber;
    }

    EventCallback *eventCallback = new EventCallback(StateEntryEventName(toState), eventObject, eventMethod, priority);
    eventCallback->subscriberData = subscriberData;
    eventSubscriber->AddCallback(eventCallback);
}

void StateManager::SubscribeStateChange(uint32 fromState, uint32 toState, IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData, int32 priority)
{
    if(fromState == 0 || toState == 0 || fromState >= validStates->size() || toState >= validStates->size())
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Invalid state handle passed to SubscribeStateChange");
        return;
    }

    EventSubscriber *eventSubscriber = stateTransitionSubscribers->at(fromState)->at(toState);

    if(eventSubscriber == null)
    {
        std::string errMsg = "SubscribeStateChange called for an invalid state transition: ";
        errMsg += validStates->at(fromState) + " to " + validStates->at(toState);

        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, errMsg.c_str());
        return;
    }

    EventCallback *eventCallback = new EventCallback(StateExitEventName(fromState), eventObject, eventMethod, priority);
    eventCallback->subscriberData = subscriberData;
    eventSubscriber->AddCallback(eventCallback);
}

void StateManager::SubscribeStateExit(uint32 fromState, IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData, int32 priority)
{
    if(fromState == 0 || fromState >= validStates->size())
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Invalid state handle passed to SubscribeStateExit");
        return;
    }

    EventSubscriber *eventSubscriber = stateExitSubscribers->at(fromState);

    if(eventSubscriber == null)
    {
        eventSubscriber = new EventSubscriber(StateExitEventName(fromState), this);
        stateExitSubscribers->at(fromState) = eventSubscriber;
    }

    EventCallback *eventCallback = new EventCallback(StateExitEventName(fromState), eventObject, eventMethod, priority);
    eventCallback->subscriberData = subscriberData;
    eventSubscriber->AddCallback(eventCallback);
}

void StateManager::NotifyGameObjectDeletion(IGameObject *gameObject)
{
    if(traceState)
    {
        s3eDebugTracePrintf("StateMgr notified of deletion of GO: %s", gameObject->debugId);
    }

    anyEntrySubscriber->NotifyGameObjectDeletion(gameObject);
    anyTransitionSubscriber->NotifyGameObjectDeletion(gameObject);
    anyExitSubscriber->NotifyGameObjectDeletion(gameObject);

    for(uint32 index = 0; index < stateEntrySubscribers->size(); index++)
    {
        EventSubscriber *checkSub = stateEntrySubscribers->at(index);
        if(checkSub != null)
        {
            checkSub->NotifyGameObjectDeletion(gameObject);
        }
    }

    for(uint32 index = 0; index < stateExitSubscribers->size(); index++)
    {
        EventSubscriber *checkSub = stateExitSubscribers->at(index);
        if(checkSub != null)
        {
            checkSub->NotifyGameObjectDeletion(gameObject);
        }
    }

    for(uint32 index = 0; index < stateTransitionSubscribers->size(); index++)
    {
        for(uint32 subIndex = 0; subIndex < stateTransitionSubscribers->at(index)->size(); subIndex++)
        {
            EventSubscriber *checkSub = stateTransitionSubscribers->at(index)->at(subIndex);
            if(checkSub != null)
            {
                checkSub->NotifyGameObjectDeletion(gameObject);
            }
        }
    }

    std::map<std::string, EventSubscriber *>::iterator it;
    for ( it=eventSubscribers->begin() ; it != eventSubscribers->end(); it++ )
    {
        EventSubscriber *checkSub = (*it).second;
        if(checkSub != null)
        {
            checkSub->NotifyGameObjectDeletion(gameObject);
        }
    }
}

