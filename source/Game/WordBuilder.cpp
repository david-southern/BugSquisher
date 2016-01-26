#include "WordBuilder.h"

#define NUM_PREFIXES 37
const char *allPrefixes[NUM_PREFIXES] =
{
     "b", "bl", "br",  "c", "ch", "cl", "cr",  "d", "dr", 
     "f", "fl",  "g", "gr",  "h",  "j",  "l",  "m",  "n", "p", 
    "pl", "pr",  "r",  "s", "sc", "sh", "sl", "sn", "sp", "st", 
   "str", "sw",  "t", "th", "tr",  "v",  "w", "wh"
};

#define NUM_SUFFIXES 79
const char *allSuffixes[NUM_SUFFIXES] =
{
    "ab", "ack", "ad", "ag", "ail", "ain", "ake", "ale", "all", 
    "am", "ame", "an", "ank", "ap", "ar", "are", "at", "ate", "ave", 
    "aw", "ay", "eak", "ear", "eat", "ed", "ee", "eed", "eep", "eer", 
    "eet", "ell", "en", "end", "ent", "est", "et", "ew", "ice", "ick", 
    "id", "ide", "ig", "ight", "ike", "ile", "ill", "im", "in", "ine", 
    "ing", "ink", "ip", "it", "ite", "ive", "ob", "ock", "og", "one", 
    "ood", "ook", "oop", "oot", "op", "ope", "ore", "ose", "ot", "out", 
    "ow", "ub", "uck", "ue", "uff", "ug", "ump", "un", "ust", "ut"
};

#define NUM_VALID_WORDS 1151
const char *allValidWords[NUM_VALID_WORDS] =
{
    "back", "bad", "bag", "bail", "bake", "bale", "ball", "ban", "bank", 
    "bar", "bare", "bat", "bate", "bay", "beak", "bear", "beat", "bed", "bee", 
    "beep", "beer", "beet", "bell", "ben", "bend", "bent", "best", "bet", "bid", 
    "bide", "big", "bight", "bike", "bile", "bill", "bin", "bit", "bite", "blab", 
    "black", "blame", "blank", "blare", "bleak", "blear", "bleat", "bled", "bleed", "bleep", 
    "blend", "blest", "blew", "blight", "blink", "blip", "blob", "block", "blog", "blood", 
    "blot", "blow", "blue", "bluff", "bob", "bog", "bone", "book", "boot", "bop", 
    "bore", "bout", "bow", "brad", "brag", "brain", "brake", "bran", "brat", "brave", 
    "bray", "break", "bred", "breed", "brew", "brick", "bride", "brig", "bright", "brim", 
    "brine", "bring", "brink", "brit", "brood", "brook", "brow", "brut", "buck", "buff", 
    "bug", "bump", "bun", "bust", "but", "cab", "cad", "cake", "call", "cam", 
    "came", "can", "cap", "car", "care", "cat", "cave", "caw", "cay", "cell", 
    "cent", "chad", "chain", "chap", "char", "chat", "chaw", "cheat", "cheep", "cheer", 
    "chest", "chew", "chick", "chid", "chide", "chile", "chill", "chin", "chine", "chink", 
    "chip", "chit", "chive", "chock", "chop", "chore", "chose", "chow", "chub", "chuck", 
    "chuff", "chug", "chump", "cite", "clack", "clad", "clam", "clan", "clank", "clap", 
    "claw", "clay", "clear", "cleat", "clew", "click", "cling", "clink", "clip", "clock", 
    "clog", "clone", "clop", "close", "clot", "clout", "club", "cluck", "clue", "clump", 
    "cob", "cog", "cone", "cook", "coop", "coot", "cop", "cope", "core", "cot", 
    "cow", "crab", "crack", "crag", "cram", "crank", "crap", "crate", "crave", "craw", 
    "creak", "creed", "creep", "crest", "crew", "crick", "crock", "crone", "crook", "crop", 
    "crow", "crust", "cub", "cue", "cuff", "cut", "dab", "dad", "dale", "dam", 
    "dame", "dank", "dare", "date", "day", "dear", "deed", "deep", "deer", "dell", 
    "den", "dent", "dew", "dice", "did", "dig", "dike", "dill", "dim", "din", 
    "dine", "ding", "dip", "dive", "dock", "dog", "done", "dope", "dose", "dot", 
    "drab", "drag", "drain", "drake", "dram", "drank", "draw", "dray", "drew", "drill", 
    "drink", "drip", "drive", "drone", "droop", "drop", "drub", "drug", "dub", "duck", 
    "due", "dug", "dump", "dun", "dust", "fad", "fail", "fain", "fake", "fall", 
    "fame", "fan", "far", "fare", "fat", "fate", "fear", "feat", "fed", "fee", 
    "feed", "feet", "fell", "fen", "fend", "few", "fig", "fight", "file", "fill", 
    "fin", "fine", "fink", "fit", "five", "flab", "flack", "flag", "flail", "flake", 
    "flame", "flank", "flap", "flare", "flat", "flaw", "flay", "fled", "flee", "fleer", 
    "fleet", "flew", "flick", "flight", "fling", "flip", "flit", "flock", "flog", "flood", 
    "flop", "flout", "flow", "flub", "flue", "fluff", "fob", "fog", "food", "foot", 
    "fop", "fore", "fun", "gab", "gad", "gag", "gain", "gale", "gall", "game", 
    "gap", "gate", "gave", "gear", "gee", "gent", "get", "gig", "gill", "gin", 
    "give", "gob", "gone", "good", "gore", "got", "gout", "grab", "grad", "grail", 
    "grain", "gram", "grate", "grave", "gray", "great", "greed", "greet", "grew", "grid", 
    "grill", "grim", "grin", "grip", "grit", "grog", "grope", "grout", "grow", "grub", 
    "gruff", "grump", "gun", "gust", "gut", "hack", "had", "hag", "hail", "hake", 
    "hale", "hall", "ham", "hank", "hap", "hare", "hat", "hate", "have", "haw", 
    "hay", "hear", "heat", "heed", "hen", "hew", "hick", "hid", "hide", "hike", 
    "hill", "him", "hip", "hit", "hive", "hob", "hock", "hog", "hone", "hood", 
    "hook", "hoop", "hoot", "hop", "hope", "hose", "hot", "how", "hub", "hue", 
    "huff", "hug", "hump", "hun", "hut", "jab", "jack", "jag", "jail", "jam", 
    "jar", "jaw", "jay", "jeep", "jeer", "jell", "jest", "jet", "jew", "jig", 
    "jive", "job", "jock", "jog", "jot", "jug", "jump", "just", "jut", "lab", 
    "lack", "lad", "lag", "lain", "lake", "lam", "lame", "lank", "lap", "late", 
    "lave", "law", "lay", "leak", "led", "lee", "leer", "lend", "lent", "lest", 
    "let", "lice", "lick", "lid", "light", "like", "line", "link", "lip", "lit", 
    "lite", "live", "lob", "lock", "log", "lone", "look", "loop", "loot", "lop", 
    "lope", "lore", "lose", "lot", "lout", "low", "luck", "luff", "lug", "lump", 
    "mad", "mail", "main", "make", "male", "mall", "man", "map", "mar", "mare", 
    "mat", "mate", "maw", "may", "meat", "meed", "meet", "men", "mend", "met", 
    "mew", "mice", "mid", "might", "mile", "mill", "mine", "ming", "mink", "mite", 
    "mob", "mock", "mood", "moot", "mop", "mope", "more", "mow", "muck", "mug", 
    "must", "nab", "nag", "nail", "name", "nap", "nave", "nay", "near", "neat", 
    "need", "nest", "net", "new", "nice", "nick", "night", "nile", "nine", "nip", 
    "nit", "nob", "nog", "none", "nook", "nope", "nose", "not", "now", "nub", 
    "nun", "nut", "pack", "pad", "pail", "pain", "pale", "pall", "pan", "pap", 
    "par", "pare", "pat", "pate", "pave", "paw", "pay", "peak", "pear", "peat", 
    "peep", "peer", "pell", "pen", "pend", "pent", "pest", "pet", "pew", "pick", 
    "pig", "pike", "pile", "pill", "pin", "pine", "ping", "pink", "pip", "pit", 
    "plain", "plan", "plank", "plat", "plate", "play", "pleat", "pled", "plight", "plop", 
    "plot", "plow", "pluck", "plug", "plump", "pock", "pone", "pop", "pope", "pore", 
    "pose", "pot", "pout", "pow", "pram", "prank", "prate", "pray", "price", "pride", 
    "prig", "prim", "prink", "prone", "prop", "prose", "prow", "pub", "puck", "puff", 
    "pug", "pump", "pun", "put", "rack", "rad", "rag", "rail", "rain", "rake", 
    "ram", "ran", "rank", "rap", "rare", "rat", "rate", "rave", "raw", "ray", 
    "rear", "red", "reed", "rend", "rent", "rest", "ret", "rice", "rick", "rid", 
    "ride", "rig", "right", "rile", "rill", "rim", "ring", "rink", "rip", "rite", 
    "rive", "rob", "rock", "rood", "rook", "root", "rope", "rose", "rot", "rout", 
    "row", "rub", "rue", "ruff", "rug", "rump", "run", "rust", "rut", "sack", 
    "sad", "sag", "sail", "sake", "sale", "same", "sank", "sap", "sat", "sate", 
    "save", "saw", "say", "scab", "scale", "scam", "scan", "scar", "scare", "scat", 
    "scent", "scone", "scoop", "scoot", "scope", "score", "scot", "scout", "scow", "scuff", 
    "sear", "seat", "see", "seed", "seep", "seer", "sell", "send", "sent", "set", 
    "sew", "shack", "shad", "shag", "shake", "shale", "shall", "sham", "shame", "shank", 
    "share", "shave", "shaw", "shay", "shear", "shed", "sheep", "sheer", "sheet", "shell", 
    "shill", "shim", "shin", "shine", "ship", "shock", "shone", "shook", "shoot", "shop", 
    "shore", "shot", "shout", "show", "shuck", "shun", "shut", "sick", "side", "sight", 
    "sill", "sin", "sine", "sing", "sink", "sip", "sit", "site", "slab", "slack", 
    "slag", "slain", "slake", "slam", "slap", "slat", "slate", "slave", "slaw", "slay", 
    "sled", "sleep", "sleet", "slew", "slice", "slick", "slid", "slide", "slight", "slim", 
    "sling", "slink", "slip", "slit", "slob", "slog", "sloop", "slop", "slope", "slot", 
    "slow", "slue", "sluff", "slug", "slump", "snack", "snag", "snail", "snake", "snap", 
    "snare", "sneak", "sneer", "snide", "snip", "snit", "snob", "snood", "snoop", "snoot", 
    "snore", "snot", "snout", "snow", "snub", "snuck", "snuff", "snug", "sob", "sock", 
    "soot", "sop", "sore", "sot", "sow", "spain", "spall", "span", "spank", "spar", 
    "spare", "spat", "spate", "spay", "speak", "spear", "sped", "speed", "spell", "spend", 
    "spent", "spew", "spice", "spike", "spill", "spin", "spine", "spit", "spite", "spook", 
    "spore", "spot", "spout", "spun", "stab", "stack", "stag", "stain", "stake", "stale", 
    "stall", "stank", "star", "stare", "state", "stave", "stay", "steak", "steed", "steep", 
    "steer", "stent", "stet", "stew", "stick", "stile", "still", "sting", "stink", "stock", 
    "stone", "stood", "stoop", "stop", "stope", "store", "stout", "stow", "strain", "strap", 
    "straw", "stray", "streak", "street", "strew", "stride", "strike", "string", "strip", "strive", 
    "strop", "struck", "strut", "stub", "stuck", "stuff", "stump", "stun", "sub", "suck", 
    "sue", "sump", "sun", "swab", "swag", "swain", "swale", "swam", "swan", "swank", 
    "swap", "swat", "sway", "swear", "sweat", "sweep", "sweet", "swell", "swig", "swill", 
    "swim", "swine", "swing", "swoop", "swop", "swore", "tab", "tack", "tad", "tag", 
    "tail", "take", "tale", "tall", "tame", "tan", "tank", "tap", "tar", "tare", 
    "tat", "taw", "teak", "tear", "teat", "ted", "tee", "teed", "tell", "ten", 
    "tend", "tent", "test", "than", "thank", "that", "thaw", "thee", "then", "thew", 
    "thick", "thin", "thine", "thing", "think", "those", "thug", "thump", "tick", "tide", 
    "tight", "tile", "till", "tin", "tine", "tip", "tog", "tone", "took", "toot", 
    "top", "tore", "tot", "tout", "tow", "track", "trail", "train", "tram", "trap", 
    "tray", "treat", "tree", "treed", "trend", "trice", "trick", "trill", "trim", "trip", 
    "trite", "troop", "trope", "trot", "trout", "truck", "true", "trump", "trust", "tub", 
    "tuck", "tuff", "tug", "tut", "vain", "vale", "van", "vat", "veer", "vend", 
    "vent", "vest", "vet", "vice", "vile", "vim", "vine", "vow", "wad", "wag", 
    "wail", "wain", "wake", "wale", "wall", "wan", "war", "ware", "wave", "way", 
    "weak", "wear", "wed", "wee", "weed", "weep", "well", "wend", "went", "west", 
    "wet", "whack", "whale", "wham", "what", "wheat", "when", "whet", "whew", "while", 
    "whim", "whine", "whip", "whit", "white", "whoop", "whose", "wick", "wide", "wig", 
    "wight", "wile", "will", "win", "wine", "wing", "wink", "wit", "wive", "wood", 
    "wore", "wow"
};
WordBuilder::WordBuilder()
{
    shuffledPrefixes = new vector<const char *>;
    shuffledSuffixes = new vector<const char *>;
    validWords = new map<string, bool>;

    for(prefixIndex = 0; prefixIndex < NUM_PREFIXES; prefixIndex++)
    {
        shuffledPrefixes->push_back(allPrefixes[prefixIndex]);
    }

    for(suffixIndex = 0; suffixIndex < NUM_SUFFIXES; suffixIndex++)
    {
        shuffledSuffixes->push_back(allSuffixes[suffixIndex]);
    }

    for(uint32 validIndex = 0; validIndex < NUM_VALID_WORDS; validIndex++)
    {
        const char *word = allValidWords[validIndex];
        string key(word);

        (*validWords)[key] = true;
    }

    ShufflePrefixes();
    ShuffleSuffixes();
}

WordBuilder::~WordBuilder()
{
    delete shuffledPrefixes;
    delete shuffledSuffixes;
    delete validWords;
}

const char *WordBuilder::GetNextPrefix()
{
    const char *retval = shuffledPrefixes->at(prefixIndex++);

    if(prefixIndex >= shuffledPrefixes->size())
    {
        ShufflePrefixes();
    }

    return retval;
}
const char *WordBuilder::GetNextSuffix()
{
    const char *retval = shuffledSuffixes->at(suffixIndex++);

    if(suffixIndex >= shuffledSuffixes->size())
    {
        ShuffleSuffixes();
    }

    return retval;
}

void WordBuilder::ShufflePrefixes()
{
    for(uint32 shuffleIndex = 0; shuffleIndex < shuffledPrefixes->size(); shuffleIndex++)
    {
        uint32 swapPos = rand() % shuffledPrefixes->size();
        const char *tmp = shuffledPrefixes->at(shuffleIndex);
        shuffledPrefixes->at(shuffleIndex) = shuffledPrefixes->at(swapPos);
        shuffledPrefixes->at(swapPos) = tmp;
    }
    prefixIndex = 0;
}

void WordBuilder::ShuffleSuffixes()
{
    for(uint32 shuffleIndex = 0; shuffleIndex < shuffledSuffixes->size(); shuffleIndex++)
    {
        uint32 swapPos = rand() % shuffledSuffixes->size();
        const char *tmp = shuffledSuffixes->at(shuffleIndex);
        shuffledSuffixes->at(shuffleIndex) = shuffledSuffixes->at(swapPos);
        shuffledSuffixes->at(swapPos) = tmp;
    }
    suffixIndex = 0;
}

bool WordBuilder::IsValidWord(const char *checkWord)
{
    map<string,bool>::iterator it;
    it=validWords->find(checkWord);

    return it != validWords->end();
}