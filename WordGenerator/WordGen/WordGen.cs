using System;
using System.Windows.Forms;

namespace BugSquisher
{
    public partial class WordGenForm : Form
    {
        public WordGenForm()
        {
            InitializeComponent();
        }

        string array_name = "";

        private void button1_Click(object sender, EventArgs e)
        {
            WordBuilder builder = new WordBuilder(3);

            WordListBox.Items.Clear();

            array_name = "all_valid_words";

            foreach (var start in builder.StartingSounds)
            {
                foreach (var end in builder.EndingSounds)
                {
                    string checkWord = start + end;

                    if (builder.IsValidWord(checkWord))
                    {
                        WordListBox.Items.Add(checkWord);
                    }
                }
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            for (int index = 0; index < WordListBox.Items.Count; index++)
            {
                WordListBox.SetSelected(index, true);
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            WordBuilder builder = new WordBuilder(3);

            WordListBox.Items.Clear();

            foreach (var start in builder.StartingSounds)
            {
                foreach (var end in builder.EndingSounds)
                {
                    string checkWord = start + end;

                    if (!builder.IsValidWord(checkWord) && GameUtils.CheckDictionaryWord(checkWord))
                    {
                        WordListBox.Items.Add(checkWord);
                    }
                }
            }

        }

        private void button2_Click(object sender, EventArgs e)
        {
            string wordCode = "const char *" + array_name + "[{0}] =" + Environment.NewLine + "{{" + Environment.NewLine + "    ";

            string wordList = "";
            int wordCount = 0;

            foreach (var saveWord in WordListBox.SelectedItems)
            {
                if (wordList.Length > 0)
                {
                    wordList += ", ";
                }

                if (wordCount++ % 10 == 9)
                {
                    wordList += Environment.NewLine + "    ";
                }

                wordList += "\"" + saveWord + "\"";
            }

            wordCode += wordList;

            if (wordCount++ % 10 != 9)
            {
                wordCode += Environment.NewLine;
            }

            wordCode += "}};";

            Clipboard.SetText(String.Format(wordCode, wordCount - 1));

            MessageBox.Show("Check your clipboard for the C++ code");
        }

        private void button3_Click(object sender, EventArgs e)
        {
            string wordList = "";
            int wordCount = 0;

            foreach (var saveWord in WordListBox.SelectedItems)
            {
                if (wordList.Length > 0)
                {
                    wordList += ", ";
                }

                wordList += "\"" + saveWord + "\"";

                if (wordCount++ % 10 == 9)
                {
                    wordList += Environment.NewLine;
                }
            }

            Clipboard.SetText(wordList);

            MessageBox.Show("Check your clipboard for the C# code");
        }

        private void button6_Click(object sender, EventArgs e)
        {
            button3_Click(sender, e);
        }

        private void button7_Click(object sender, EventArgs e)
        {
            WordBuilder builder = new WordBuilder(3);

            WordListBox.Items.Clear();

            array_name = "all_prefixes";

            foreach (var start in builder.StartingSounds)
            {
                WordListBox.Items.Add(start);
            }

        }

        private void button8_Click(object sender, EventArgs e)
        {
            WordBuilder builder = new WordBuilder(3);

            WordListBox.Items.Clear();

            array_name = "all_suffixes";

            foreach (var end in builder.EndingSounds)
            {
                WordListBox.Items.Add(end);
            }

        }
    }
}
