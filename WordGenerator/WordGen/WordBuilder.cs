using System.Collections.Generic;

namespace BugSquisher
{
    public class WordBuilder
    {
        public List<string> StartingSounds;
        public List<string> EndingSounds;

        private string[] all_starting_sounds = new string[]
        { 
            "b", "bl", "br", "c", "ch", "cl", "cr", "d", "dr",
            "f", "fl", "g", "gr", "h", "j", "l", "m", "n",
            "p", "pl", "pr", "r", "s", "sc", "sh", "sl", "sn",
            "sp", "st", "str", "sw", "t", "th", "tr", "v", "w", "wh"

        };

        private string[] beginner_starting_sounds = new string[]
        {
            "b", "c", "ch", "d", "f", "g", "h", "j", "l", "m", "n",
            "r", "s", "sh", "t", "th", "v", "w", "w", "wh"
        };

        string[] all_ending_sounds = new string[]
        {
            "ab", "ack", "ad", "ag", "ail", "ain", "ake", "ale",
            "all", "am", "ame", "an", "ank", "ap", "ar", "are",
            "at", "ate", "ave", "aw", "ay", "eak", "ear", "eat",
            "ed", "ee", "eed", "eep", "eer", "eet", "ell", "en",
            "end", "ent", "est", "et", "ew", "ice", "ick", "id",
            "ide", "ig", "ight", "ike", "ile", "ill", "im", "in",
            "ine", "ing", "ink", "ip", "it", "ite", "ive", "ob",
            "ock", "og", "one", "ood", "ook", "oop", "oot", "op",
            "ope", "ore", "ose", "ot", "out", "ow", "ub", "uck",
            "ue", "uff", "ug", "ump", "un", "ust", "ut"
        };

        private string[] beginner_ending_sounds = new string[]
        {
            "ack", "ad", "ag", "ail", "ake", "all", "ame", "an",
            "ap", "ar", "are", "at", "ate", "ave", "aw", "ear",
            "eat", "ed", "eed", "ell", "en", "end", "ent", "est",
            "et", "ice", "ick", "ide", "ig", "ight", "ike", "ill",
            "in", "ine", "ing", "ink", "ip", "it", "ob", "ock",
            "og", "one", "ook", "oot", "op", "ope", "ose", "ot",
            "ow", "ub", "ug", "ump", "un", "ut"
        };

        private string[] generate_starting_sounds;
        private string[] generate_ending_sounds;

        Dictionary<string, bool> IgnoredWords = new Dictionary<string, bool>();
        string[] ignored_words = new string[]
        {
            "bock", "dow", "gop", "jan", "jim", "jill", "lat",
            "mag", "med", "mell", "mig", "min", "mit", "pob", "sam",
            "san", "tay", "tim", "veep", "vide", "vip", "cree", "blent", "brab",
            "drear", "grot", "prot", "scag", "scop", "stan", "stat",
            "stook", "swed", "trop", "vail", "tue", "bing", "dan", "fide",
            "jake", "jose", "tope", "jun",
            "blain", "brill", "cit","fay", "gar","gen" ,"ling" ,"mike", "prob",
            "prog" ,"sen" ,"shew" ,"sid", "trig","vive" ,"whig", "lust", "poop"
 
        };

        Dictionary<string, bool> AllowedWords = new Dictionary<string, bool>();
        string[] allowed_words = new string[]
        {
        };

        public WordBuilder(int difficulty)
        {
            if (difficulty == 0)
            {
                StartingSounds = new List<string>(beginner_starting_sounds);
                EndingSounds = new List<string>(beginner_ending_sounds);
            }
            else
            {
                StartingSounds = new List<string>(all_starting_sounds);
                EndingSounds = new List<string>(all_ending_sounds);
            }

            StartingSounds.Sort();
            EndingSounds.Sort();

            generate_starting_sounds = StartingSounds.ToArray();
            generate_ending_sounds = EndingSounds.ToArray();

            foreach (string ignored_word in ignored_words)
            {
                IgnoredWords.Add(ignored_word, true);
            }

            foreach (string allowed_word in allowed_words)
            {
                AllowedWords.Add(allowed_word, true);
            }

            InitializeGame();
        }

        public void InitializeGame()
        {
            GameUtils.Shuffle<string>(EndingSounds);
            GameUtils.Shuffle<string>(StartingSounds);
        }

        public bool IsValidWord(string check_word)
        {
            return GameUtils.CheckDictionaryWord(check_word)
                && (!GameUtils.IsCensoredWord(check_word) || AllowedWords.ContainsKey(check_word))
                && !IsIgnoredWord(check_word);
        }

        private bool IsIgnoredWord(string check_word)
        {
            return IgnoredWords.ContainsKey(check_word);
        }
    }
}
