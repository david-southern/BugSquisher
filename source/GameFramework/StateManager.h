#pragma once

#include <vector>
#include <string>
#include <map>
#include <queue>

#include "s3e.h"
#include "Iw2D.h"
#include "IwResManager.h"

#include "GameInterfaces.h"

class StateManager;
class EventCallback;

typedef  void (IGameObject::*EventCallbackPtr)(EventCallback *eventData);
#define CALL_MEMBER_FN(objectPtr,methodPtr)  ((objectPtr)->*(methodPtr))


union StateVariable
{
    bool boolValue;
    int32 intValue;
    unsigned int uintValue;
    float floatValue;
    const char *stringValue;
    void *userValue;
};

class VarEventData
{
public:
    int varHandle;
    std::string varName;
    StateVariable oldValue;
    StateVariable newValue;
};

class EventCallback
{
public:
    EventCallback()
    {
        eventName = "";
        eventObject = null;
        eventMethod = null;
        eventData = null;
        subscriberData = null;
        gameTime = 0;
        priority = 0;
    }

    EventCallback(std::string eventName, IGameObject *eventObject, EventCallbackPtr eventMethod, int32 priority);

    void Dispatch(uint32 gameTime, void *eventData = null);

    std::string eventName;
    uint32 gameTime;
    IGameObject *eventObject;
    EventCallbackPtr eventMethod;
    int priority;
    void *eventData;
    void *subscriberData;
};


class EventSubscriber
{
    StateManager *state;
    std::string eventName;
    std::vector<EventCallback *> *callbacks;
    bool needsSort;

public:
    EventSubscriber(std::string eventName, StateManager *state);
    ~EventSubscriber();

    void AddCallback(EventCallback *callback);
    void Dispatch(uint32 gameTime, void *eventData);
    void NotifyGameObjectDeletion(IGameObject *gameObject);
};

class GameManager;

class StateChangeData
{
public:
    uint32 fromState;
    std::string fromStateName;
    uint32 toState;
    std::string toStateName;
    bool cancelStateChange;
};

class StateManager
{
    GameManager *mgr;

    std::vector<std::string> *validStates;

    EventSubscriber *anyEntrySubscriber;
    EventSubscriber *anyTransitionSubscriber;
    EventSubscriber *anyExitSubscriber;

    std::vector<EventSubscriber *> *stateEntrySubscribers;
    std::vector<EventSubscriber *> *stateExitSubscribers;
    std::vector<std::vector<EventSubscriber *> *> *stateTransitionSubscribers;
    uint32 currentState;

    std::vector<std::string> *varNames;
    std::vector<StateVariable> *vars;
    std::map<std::string, EventSubscriber*> *eventSubscribers;
    std::vector<EventCallback *> *futureEventList;
    std::vector<EventCallback *> *postClearFutureEventList;
    bool felNeedsSort;
    bool clearFelPending;

    void DispatchVarChangeEvent(uint32 varHandle, VarEventData *eventData);
    std::string StateEntryEventName(uint32 stateHandle);
    std::string StateChangeEventName(uint32 fromState, uint32 toState);
    std::string StateExitEventName(uint32 stateHandle);

public:
    static const std::string INVALID_STATE;
    static const std::string INVALID_VARIABLE;
    static StateVariable zeroVar;


    StateManager(GameManager *mgr);
    ~StateManager();

    GameManager *GetGameManager();

    uint32 RegisterGameState(std::string stateName);
    void DefineValidStateTransition(uint32 fromState, uint32 toState);
    void SetInitialState(uint32 initialState);
    bool ChangeState(uint32 newState);
    uint32 GetCurrentState();
    std::string GetStateName(uint32 state);

    void SubscribeAnyStateEntry(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null, int32 priority = 0);
    void SubscribeAnyStateChange(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null, int32 priority = 0);
    void SubscribeAnyStateExit(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null, int32 priority = 0);

    void SubscribeStateEntry(uint32 toState, IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null, int32 priority = 0);
    void SubscribeStateChange(uint32 fromState, uint32 toState, IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null, int32 priority = 0);
    void SubscribeStateExit(uint32 fromState, IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null, int32 priority = 0);

    uint32 RegisterVariable(std::string varName, StateVariable initialValue);
    StateVariable GetVar(uint32 varHandle);
    void SetVar(uint32 varHandle, StateVariable value);

    void SubscribeEvent(std::string eventName, IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null, int32 priority = 0);
    void SubscribeStateVarChange(uint32 varHandle, IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null, int32 priority = 0);

    void PostEvent(std::string eventName, void *eventData = null, uint32 gameTime = 0);
    void PostFutureEvent(std::string eventName, uint32 eventTimeMillis, void *eventData = null, int32 priority = 0);
    void ClearFutureEvents();

    void Update(uint32 gameTime);

    void NotifyGameObjectDeletion(IGameObject *gameObject);
};


