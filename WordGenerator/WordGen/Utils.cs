using System;
using System.Diagnostics;
using System.IO;
using System.Security;
using System.Security.Cryptography;
using System.Text;
using System.Threading;

namespace BugSquisher
{
    [Serializable]
    public class ExceptionEx : Exception
    {
        public ExceptionEx(string formatMessage, params object[] formatParams)
            : base(formatParams.Length > 0 ? String.Format(formatMessage, formatParams) : formatMessage)
        {
        }
    }

    public static class Utils
    {
        public static readonly int DEFAULT_SHORT_STRING_LENGTH = 5000;

        /// <summary>
        /// Returns a string guaranteed to be shorter than the max_length parameter.  If the
        /// input string is already shorter, then it is returned unchanged, otherwise a new
        /// truncated string is returned.
        /// </summary>
        /// <param name="long_string">The string to be shortened</param>
        /// <param name="max_length">The maximum length of the string</param>
        public static string ShortString(string long_string, int max_length)
        {
            if (long_string.Length < max_length)
            {
                return long_string;
            }

            return long_string.Substring(0, max_length);
        }

        /// <summary>
        /// Returns a string guaranteed to be shorter than the max_length parameter considering byte count in the string.  If the
        /// input string is already shorter, then it is returned unchanged, otherwise a new
        /// truncated string is returned.
        /// </summary>
        /// <param name="long_string">The string to be shortened</param>
        /// <param name="max_length">The maximum length of the string</param>
        public static string ShortStringBytes(string long_string, int max_length)
        {
            // convert it to bytes and truncate it, to handle any multibyte or non ascii characters in there
            System.Text.Encoder encode = System.Text.Encoding.UTF8.GetEncoder();
            char[] keyChars = long_string.ToCharArray();
            int charCount = encode.GetByteCount(keyChars, 0, keyChars.Length, true);
            if (charCount > max_length)
            {
                byte[] buffer = new byte[charCount];
                encode.GetBytes(keyChars, 0, keyChars.Length, buffer, 0, true);
                // strip the key down to required size, only decode max_length
                System.Text.Decoder decode = System.Text.Encoding.UTF8.GetDecoder();
                decode.Fallback = new System.Text.DecoderReplacementFallback("");
                charCount = decode.GetCharCount(buffer, 0, max_length);
                char[] ca = new char[charCount];
                decode.GetChars(buffer, 0, max_length, ca, 0);
                string newkey = new String(ca);
                return newkey;
            }
            else
            {
                return long_string;
            }
        }

        /// <summary>
        /// Returns a string guaranteed to be "short", meaning that the string is less than
        /// the DEFAULT_SHORT_STRING_LENGTH constant defined in this Utils class.  If the
        /// input string is already shorter, then it is returned unchanged, otherwise a new
        /// truncated string is returned.
        /// </summary>
        /// <param name="long_string">The string to be shortened</param>
        public static string ShortString(string long_string)
        {
            return ShortString(long_string, DEFAULT_SHORT_STRING_LENGTH);
        }

        /// <summary>
        /// Returns the input object converted to a string, or the default_value parameter if the
        /// input was null or not convertible to a string.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static string SafeString(object thingy, string default_value)
        {
            if (thingy == null)
            {
                return default_value;
            }

            try
            {
                return thingy.ToString();
            }
            catch (Exception)
            { }

            return default_value;
        }

        /// <summary>
        /// Returns the input object converted to an integer, or the default_value parameter if the
        /// input was null or not convertible to an integer.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static bool SafeBool(object thingy, bool default_value)
        {
            if (thingy == null)
            {
                return default_value;
            }

            try
            {
                if (thingy is string)
                {
                    string bool_str = thingy.ToString().ToLower().Trim();

                    if (bool_str == Boolean.TrueString.ToLower())
                    {
                        return true;
                    }

                    return bool_str == "1" || bool_str == "t" || bool_str == "y"
                      || bool_str == "true" || bool_str == "yes";
                }
                else if (thingy is bool)
                {
                    return ((bool)thingy);
                }
                else if (thingy is char)
                {
                    char bool_char = Char.ToLower((char)thingy);

                    return bool_char == '1' || bool_char == 't' || bool_char == 'y';
                }
                else if (thingy is byte)
                {
                    return (byte)thingy != 0;
                }
                else if (thingy is sbyte)
                {
                    return (sbyte)thingy != 0;
                }
                else if (thingy is short)
                {
                    return (short)thingy != 0;
                }
                else if (thingy is ushort)
                {
                    return (ushort)thingy != 0;
                }
                else if (thingy is int)
                {
                    return (int)thingy != 0;
                }
                else if (thingy is uint)
                {
                    return (uint)thingy != 0;
                }
                else if (thingy is long)
                {
                    return (long)thingy != 0;
                }
                else if (thingy is ulong)
                {
                    return (ulong)thingy != 0;
                }
                else if (thingy is decimal)
                {
                    return (decimal)thingy != 0;
                }
                else if (thingy is float)
                {
                    return (float)thingy != 0;
                }
                else if (thingy is double)
                {
                    return (double)thingy != 0;
                }
            }
            catch (Exception)
            { }

            return default_value;
        }

        public static bool SafeBool(object thingy)
        {
            return SafeBool(thingy, false);
        }

        /// <summary>
        /// Returns the input object converted to an integer, or the default_value parameter if the
        /// input was null or not convertible to an integer.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static int SafeInt(object thingy, int default_value)
        {
            if (thingy == null)
            {
                return default_value;
            }

            try
            {
                if (thingy is string)
                {
                    return int.Parse((string)thingy);
                }
                else if (thingy is char)
                {
                    return (int)(char)thingy;
                }
                else if (thingy is byte)
                {
                    return (int)(byte)thingy;
                }
                else if (thingy is sbyte)
                {
                    return (int)(sbyte)thingy;
                }
                else if (thingy is short)
                {
                    return (int)(short)thingy;
                }
                else if (thingy is ushort)
                {
                    return (int)(ushort)thingy;
                }
                else if (thingy is int)
                {
                    return (int)thingy;
                }
                else if (thingy is uint)
                {
                    return (int)(uint)thingy;
                }
                else if (thingy is long)
                {
                    return (int)(long)thingy;
                }
                else if (thingy is ulong)
                {
                    return (int)(ulong)thingy;
                }
                else if (thingy is decimal)
                {
                    return (int)(decimal)thingy;
                }
                else if (thingy is float)
                {
                    return (int)(float)thingy;
                }
                else if (thingy is double)
                {
                    return (int)(double)thingy;
                }
                else if (thingy is bool)
                {
                    return ((bool)thingy) ? 1 : 0;
                }
            }
            catch (Exception)
            { }

            return default_value;
        }

        /// <summary>
        /// Returns the input object converted to a long, or the default_value parameter if the
        /// input was null or not convertible to a long.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static long SafeLong(object thingy, long default_value)
        {
            if (thingy == null)
            {
                return default_value;
            }

            try
            {
                if (thingy is string)
                {
                    return long.Parse((string)thingy);
                }
                else if (thingy is char)
                {
                    return (long)(char)thingy;
                }
                else if (thingy is byte)
                {
                    return (long)(byte)thingy;
                }
                else if (thingy is sbyte)
                {
                    return (long)(sbyte)thingy;
                }
                else if (thingy is short)
                {
                    return (long)(short)thingy;
                }
                else if (thingy is ushort)
                {
                    return (long)(ushort)thingy;
                }
                else if (thingy is int)
                {
                    return (long)(int)thingy;
                }
                else if (thingy is uint)
                {
                    return (long)(uint)thingy;
                }
                else if (thingy is long)
                {
                    return (long)thingy;
                }
                else if (thingy is ulong)
                {
                    return (long)(ulong)thingy;
                }
                else if (thingy is decimal)
                {
                    return (long)(decimal)thingy;
                }
                else if (thingy is float)
                {
                    return (long)(float)thingy;
                }
                else if (thingy is double)
                {
                    return (long)(double)thingy;
                }
                else if (thingy is bool)
                {
                    return ((bool)thingy) ? 1 : 0;
                }
            }
            catch (Exception)
            { }

            return default_value;
        }

        /// <summary>
        /// Returns the input object converted to a float, or the default_value parameter if the
        /// input was null or not convertible to a float.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static float SafeFloat(object thingy, float default_value)
        {
            if (thingy == null)
            {
                return default_value;
            }

            try
            {
                if (thingy is string)
                {
                    return float.Parse((string)thingy);
                }
                else if (thingy is char)
                {
                    return (float)(char)thingy;
                }
                else if (thingy is byte)
                {
                    return (float)(byte)thingy;
                }
                else if (thingy is sbyte)
                {
                    return (float)(sbyte)thingy;
                }
                else if (thingy is short)
                {
                    return (float)(short)thingy;
                }
                else if (thingy is ushort)
                {
                    return (float)(ushort)thingy;
                }
                else if (thingy is int)
                {
                    return (float)(int)thingy;
                }
                else if (thingy is uint)
                {
                    return (float)(uint)thingy;
                }
                else if (thingy is long)
                {
                    return (float)(long)thingy;
                }
                else if (thingy is ulong)
                {
                    return (float)(ulong)thingy;
                }
                else if (thingy is decimal)
                {
                    return (float)(decimal)thingy;
                }
                else if (thingy is float)
                {
                    return (float)thingy;
                }
                else if (thingy is double)
                {
                    return (float)(double)thingy;
                }
                else if (thingy is bool)
                {
                    return ((bool)thingy) ? 1 : 0;
                }
            }
            catch (Exception)
            { }

            return default_value;
        }

        /// <summary>
        /// Returns the input object converted to a double, or the default_value parameter if the
        /// input was null or not convertible to a double.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static double SafeDouble(object thingy, double default_value)
        {
            if (thingy == null)
            {
                return default_value;
            }

            try
            {
                if (thingy is string)
                {
                    return double.Parse((string)thingy);
                }
                else if (thingy is char)
                {
                    return (double)(char)thingy;
                }
                else if (thingy is byte)
                {
                    return (double)(byte)thingy;
                }
                else if (thingy is sbyte)
                {
                    return (double)(sbyte)thingy;
                }
                else if (thingy is short)
                {
                    return (double)(short)thingy;
                }
                else if (thingy is ushort)
                {
                    return (double)(ushort)thingy;
                }
                else if (thingy is int)
                {
                    return (double)(int)thingy;
                }
                else if (thingy is uint)
                {
                    return (double)(uint)thingy;
                }
                else if (thingy is long)
                {
                    return (double)(long)thingy;
                }
                else if (thingy is ulong)
                {
                    return (double)(ulong)thingy;
                }
                else if (thingy is decimal)
                {
                    return (double)(decimal)thingy;
                }
                else if (thingy is float)
                {
                    return (double)(float)thingy;
                }
                else if (thingy is double)
                {
                    return (double)thingy;
                }
                else if (thingy is bool)
                {
                    return ((bool)thingy) ? 1 : 0;
                }
            }
            catch (Exception)
            { }

            return default_value;
        }

        /// <summary>
        /// Returns the input object converted to a DateTime, or the default_value parameter if the
        /// input was null or not parsable as a DateTime.  (Currently only string types are parsable)
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static DateTime SafeDateTime(object thingy, DateTime default_value)
        {
            if (thingy == null)
            {
                return default_value;
            }

            try
            {
                if (thingy is DateTime)
                {
                    return (DateTime)thingy;
                }

                if (thingy is string)
                {
                    return DateTime.Parse((string)thingy);
                }
            }
            catch (Exception)
            { }

            return default_value;
        }

        /// <summary>
        /// Returns the input object converted to a string, or an empty string if the
        /// input was null or not convertible to a string.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static string SafeString(object thingy)
        {
            return SafeString(thingy, "");
        }

        /// <summary>
        /// Returns the input object converted to an integer, or int.MaxValue if the
        /// input was null or not convertible to an integer.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static int SafeInt(object thingy)
        {
            return SafeInt(thingy, int.MaxValue);
        }

        /// <summary>
        /// Returns the input object converted to a long, or long.MaxValue if the
        /// input was null or not convertible to a long.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static long SafeLong(object thingy)
        {
            return SafeLong(thingy, long.MaxValue);
        }

        /// <summary>
        /// Returns the input object converted to a float, or float.MaxValue if the
        /// input was null or not convertible to a float.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static float SafeFloat(object thingy)
        {
            return SafeFloat(thingy, float.MaxValue);
        }

        /// <summary>
        /// Returns the input object converted to a double, or double.MaxValue if the
        /// input was null or not convertible to a double.
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static double SafeDouble(object thingy)
        {
            return SafeDouble(thingy, double.MaxValue);
        }

        /// <summary>
        /// Returns the input object converted to a DateTime, or DateTime.MaxValue if the
        /// input was null or not parsable as a DateTime.  (Currently only string types are parsable)
        /// </summary>
        /// <param name="thingy">The object to convert</param>
        public static DateTime SafeDateTime(object thingy)
        {
            return SafeDateTime(thingy, DateTime.MaxValue);
        }

        /// <summary>
        /// Returns true if the input is a numeric value type, and is equal to the maximum value
        /// of the its type 
        /// </summary>
        public static bool IsMaxValue(object thingy)
        {
            if (thingy == null || thingy is string || thingy is bool)
            {
                return false;
            }

            try
            {
                if (thingy is char) { return (char)thingy == char.MaxValue; }
                if (thingy is byte) { return (byte)thingy == byte.MaxValue; }
                if (thingy is sbyte) { return (sbyte)thingy == sbyte.MaxValue; }
                if (thingy is decimal) { return (decimal)thingy == decimal.MaxValue; }
                if (thingy is short) { return (short)thingy == short.MaxValue; }
                if (thingy is ushort) { return (ushort)thingy == ushort.MaxValue; }
                if (thingy is int) { return (int)thingy == int.MaxValue; }
                if (thingy is uint) { return (uint)thingy == uint.MaxValue; }
                if (thingy is long) { return (long)thingy == long.MaxValue; }
                if (thingy is ulong) { return (ulong)thingy == ulong.MaxValue; }
                if (thingy is float) { return (float)thingy == float.MaxValue; }
                if (thingy is double) { return (double)thingy == double.MaxValue; }
            }
            catch (Exception)
            { }

            return false;
        }

        private static readonly float FLOAT_THRESHOLD = float.Epsilon * 2;

        /// <summary>
        /// Does a floating point safe comparson, comparing the two values within a 
        /// "fudge factor" to eliminate spurious differences resulting from the inherent
        /// error in floating point types.
        /// </summary>
        public static bool FloatLT(double first, double second)
        {
            return (first - second) < -FLOAT_THRESHOLD;
        }

        /// <summary>
        /// Does a floating point safe comparson, comparing the two values within a 
        /// "fudge factor" to eliminate spurious differences resulting from the inherent
        /// error in floating point types.
        /// </summary>
        public static bool FloatLE(double first, double second)
        {
            return (first - second) < FLOAT_THRESHOLD;
        }

        /// <summary>
        /// Does a floating point safe comparson, comparing the two values within a 
        /// "fudge factor" to eliminate spurious differences resulting from the inherent
        /// error in floating point types.
        /// </summary>
        public static bool FloatGT(double first, double second)
        {
            return (first - second) > FLOAT_THRESHOLD;
        }

        /// <summary>
        /// Does a floating point safe comparson, comparing the two values within a 
        /// "fudge factor" to eliminate spurious differences resulting from the inherent
        /// error in floating point types.
        /// </summary>
        public static bool FloatGE(double first, double second)
        {
            return (first - second) > -FLOAT_THRESHOLD;
        }

        /// <summary>
        /// Does a floating point safe comparson, comparing the two values within a 
        /// "fudge factor" to eliminate spurious differences resulting from the inherent
        /// error in floating point types.
        /// </summary>
        public static bool FloatEQ(double first, double second)
        {
            double value = (first - second);

            return (value < FLOAT_THRESHOLD) && (value > -FLOAT_THRESHOLD);
        }

        /// <summary>
        /// Does a floating point safe comparson, comparing the two values within a 
        /// "fudge factor" to eliminate spurious differences resulting from the inherent
        /// error in floating point types.
        /// </summary>
        public static bool FloatNE(double first, double second)
        {
            double value = (first - second);

            return (value >= FLOAT_THRESHOLD) || (value <= -FLOAT_THRESHOLD);
        }

        private static byte[] ENC_Key = { 
            53,   116,  209,   42,  200,   93,  254,   38,
            189,   84,  247,   96,  234,   93,   73,  199,
            160,  135,  215,   73,  108,  103,  193,  136,
            140,  110,   12,  131,  217,  108,  103,  117
        };

        private static byte[] ENC_IV = { 
            1,     17,  114,  188,  102,  127,   60,  209,
            207,  115,   37,   57,  171,  62,   130,   43
        };

        private static Rijndael rijn = null;
        private static ICryptoTransform encryptor = null;
        private static ICryptoTransform decryptor = null;

        private static void SetupEncryption()
        {
            rijn = Rijndael.Create();
            rijn.KeySize = ENC_Key.Length * 8;
            rijn.Key = ENC_Key;
            rijn.IV = ENC_IV;
            rijn.Padding = PaddingMode.ISO10126;
            rijn.Mode = CipherMode.CBC;

            encryptor = rijn.CreateEncryptor();
            decryptor = rijn.CreateDecryptor();
        }

        public static string SimpleEncrypt(string clear_text)
        {
            if (rijn == null)
            {
                SetupEncryption();
            }

            MemoryStream ms = new MemoryStream();

            CryptoStream crypt_stream = new CryptoStream(ms, encryptor, CryptoStreamMode.Write);

            byte[] clear_buffer = UTF8Encoding.UTF8.GetBytes(clear_text);

            crypt_stream.Write(clear_buffer, 0, clear_buffer.Length);
            crypt_stream.FlushFinalBlock();
            crypt_stream.Clear();
            crypt_stream.Close();

            return Convert.ToBase64String(ms.ToArray());
        }

        public static string SimpleDecrpyt(string crypt_text)
        {
            if (rijn == null)
            {
                SetupEncryption();
            }

            byte[] crypt_buffer = Convert.FromBase64String(crypt_text);
            MemoryStream ms = new MemoryStream(crypt_buffer);

            CryptoStream crypt_stream = new CryptoStream(ms, decryptor, CryptoStreamMode.Read);

            byte[] clear_buffer = new byte[crypt_buffer.Length];

            crypt_stream.Read(clear_buffer, 0, clear_buffer.Length);

            crypt_stream.Clear();
            crypt_stream.Close();

            int end_index;
            for (end_index = clear_buffer.Length - 1; end_index > 0 && clear_buffer[end_index] == 0; end_index--)
                ;

            byte[] trunc_buffer = new byte[end_index + 1];

            Array.Copy(clear_buffer, trunc_buffer, end_index + 1);

            return UTF8Encoding.UTF8.GetString(trunc_buffer);
        }

        private static object event_log_lock = new object();
        private static EventLog event_logger = null;

        public static void LogEventLog(EventLogEntryType event_type, int event_id, short category,
          string format_message, params object[] message_objects)
        {
            LogEventLog(String.Format(format_message, message_objects), event_type, event_id, category);
        }

        public static bool CheckEventSourceExists(string source_name)
        {
            try
            {
                return EventLog.SourceExists(source_name);
            }
            catch (SecurityException)
            {
                // Exception message: source was not found, but some or all of the event logs could not be searched
                // this happens when the user does not have privleges to search an event source, probably the Security log
                // since we will always have permssion to search the source we created, this means the source must not
                // exist
            }

            return false;
        }

        public static void LogEventLog(string message, EventLogEntryType event_type, int event_id, short category)
        {
            lock (event_log_lock)
            {
                if (event_logger == null)
                {
                    if (!CheckEventSourceExists("ELF"))
                    {
                        EventLog.CreateEventSource("ELF", "ELFLogger");

                        for (int wait_count = 0; wait_count < 30; wait_count++)
                        {
                            Thread.Sleep(1000);
                            if (CheckEventSourceExists("ELF"))
                            {
                                break;
                            }
                        }
                    }

                    event_logger = new EventLog("ELFLogger");
                    event_logger.Source = "ELF";
                }

                string event_message = String.Format("{0}  Process {1}, Thread {2}: {3}",
                  DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff"),
                  Process.GetCurrentProcess().ProcessName,
                  Process.GetCurrentProcess().Id, message);
                event_logger.WriteEntry(event_message, event_type, event_id, category);
            }
        }

        private static readonly int GROUPS_PER_CODE = 3;
        private static readonly int CHARS_PER_GROUP = 4;
        private static readonly string ValidCharacters = "ABCDEFGHJKLMNPQRSTWXYZ3456789";

        public static string GenerateUnambiguousSerialNumber()
        {
            string retval = "";

            byte[] randVal = new byte[1];

            RNGCryptoServiceProvider rand = new RNGCryptoServiceProvider();

            int maxUnbiasedValue = (255 / ValidCharacters.Length) * ValidCharacters.Length - 1;
            for (int groups = 0; groups < GROUPS_PER_CODE; groups++)
            {
                if (groups != 0)
                {
                    retval += "-";
                }

                for (int charIndex = 0; charIndex < CHARS_PER_GROUP; charIndex++)
                {
                    int randNum = int.MaxValue;

                    while (randNum > maxUnbiasedValue)
                    {
                        rand.GetBytes(randVal);
                        randNum = Convert.ToInt32(randVal[0]);
                    }

                    randNum %= ValidCharacters.Length;

                    retval += ValidCharacters[randNum];
                }
            }

            return retval;
        }
    }

    public class Pair
    {
        public object first = null, second = null;

        public Pair(object _first, object _second)
        {
            first = _first;
            second = _second;
        }
    }

    public class Triple
    {
        public object first = null, second = null, third = null;

        public Triple(object _first, object _second, object _third)
        {
            first = _first;
            second = _second;
            third = _third;
        }
    }
}
