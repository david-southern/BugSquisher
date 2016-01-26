using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using BugSquisher.Properties;

namespace BugSquisher
{
    class GameUtils
    {
        public static void Shuffle<T>(List<T> list)
        {
            Random my_rand = new Random();

            for (int shuffle_index = 0; shuffle_index < list.Count - 1; shuffle_index++)
            {
                int swap_index = my_rand.Next(shuffle_index, list.Count);
                T tmp = list[shuffle_index];
                list[shuffle_index] = list[swap_index];
                list[swap_index] = tmp;
            }
        }

        private static List<string> CensoredWords = null;
        private static Dictionary<string, bool> CensoredDict = null;

        private static void LoadCensoredWords()
        {
            CensoredWords = new List<string>();
            string[] bad_words = Resources.REALLY_BAD_WORDS___DO_NOT_READ_THIS.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

            CensoredDict = new Dictionary<string, bool>();

            foreach (string bad_word in bad_words)
            {
                CensoredDict[bad_word] = true;
            }

            CensoredWords.AddRange(CensoredDict.Keys);
        }

        public static bool IsCensoredWord(string check_word)
        {
            if (CensoredDict == null)
            {
                LoadCensoredWords();
            }

            return CensoredDict.ContainsKey(check_word);
        }

        public static string CensorText(string text)
        {
            if (CensoredWords == null)
            {
                LoadCensoredWords();
            }

            if (String.IsNullOrEmpty(text))
            {
                return text;
            }

            string censoredText = text;
            foreach (string censoredPattern in CensoredWords)
            {
                string regexPattern = ToRegexPattern(censoredPattern);
                censoredText = Regex.Replace(censoredText, regexPattern, "", RegexOptions.IgnoreCase | RegexOptions.CultureInvariant);
            }

            return censoredText;
        }

        private static string ToRegexPattern(string wildcardSearch)
        {
            string regexPattern = Regex.Escape(wildcardSearch);
            regexPattern = regexPattern.Replace(@"\*", ".*?");
            regexPattern = regexPattern.Replace(@"\?", ".");
            if (regexPattern.StartsWith(".*?"))
            {
                regexPattern = regexPattern.Substring(3);
                regexPattern = @"(^\b)*?" + regexPattern;
            }
            regexPattern = @"\b" + regexPattern + @"\b";
            return regexPattern;
        }

        private static string StarCensoredMatch(Match m)
        {
            string word = m.Captures[0].Value;
            return new string('*', word.Length);
        }

        private static Dictionary<string, bool> valid_words = null;

        public static bool CheckDictionaryWord(string check_word)
        {
            if (valid_words == null)
            {
                InitializeDictionary();
            }

            return valid_words.ContainsKey(check_word.ToLower());
        }

        private static void InitializeDictionary()
        {
            valid_words = new Dictionary<string, bool>();
            string[] word_list = Resources.US_DICTIONARY.Split(new string[] { "\n" }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string word in word_list)
            {
                valid_words.Add(word.ToLower(), true);
            }
        }
    }
}
