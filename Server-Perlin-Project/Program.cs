using Microsoft.VisualBasic.FileIO;
using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading;

namespace Server_Test
{
	public struct Parameter
	{
		public int size_x, size_y, subsections_x, subsections_y, seed, amount, discard;
	}

	//enum CMD: commandNumber to determine the received Value.
	enum PARAM_NR 
	{
		// Parameter
		SIZE_X, SIZE_Y, SUBSECTIONS_X, SUBSECTIONS_Y, SEED, AMOUNT, DISCARDS
	}
	
	enum CMD_NR
	{
		// Other messages or commands
		FILENAME = 7, TIMING
	}
	class Program
	{
		static MD5 md5 = new MD5CryptoServiceProvider();
		const int P_SIZE = 1024;
		static Parameter param;
		
		static int port;
		static Socket handler;
		static Stopwatch calculation_timer = new Stopwatch();

		private static String errormessage = "A socketoperation caused an error. Terminate.";

		static void Main(string[] args)
		{
			
			param.size_x = 1024;
			param.size_y = 1024;
			param.subsections_x = 2;
			param.subsections_y = 2;
			param.seed = 2000;
			param.amount = 1;

			Connect();
			GetParams();

			StartPerlinNoiseApp();
			SendFiles();
			Console.WriteLine("Program terminates in approx. 10 seconds.");
			handler.Close(10);
			

		}

		static void GetParams()
		{
			bool[] paramSet = new bool[Enum.GetValues(typeof(PARAM_NR)).Length];

			// Check if all parameters are set.
			while (paramSet.All(x => x) != true)
			{
				// 1. Receive instructions from Client
				byte[] message = ReceiveMessage();
				int commandType = message[0];
				string paramValue = Encoding.ASCII.GetString(message, 1, P_SIZE - 1);

				// 2. Send echo for checking, if the correct vaules were transmitted.
				SendMessage(message);

				// 3. Get Validation for Data Package
				byte[] validateMessage = ReceiveMessage();

				if (Enumerable.SequenceEqual(message, validateMessage))
				{
					paramSet[commandType] = true;
				}
				else
				{
					Console.WriteLine("Debug Message");
					Console.WriteLine("message: {0}", Encoding.ASCII.GetString(message));
					Console.WriteLine("validatemessage: {0}", Encoding.ASCII.GetString(validateMessage));
					Console.WriteLine("messagelength: " + message.Length);
					Console.WriteLine("validatemessagelength: " + validateMessage.Length);
					Console.WriteLine("message(b): ");
					foreach (byte b in message)
					{
						Console.Write(b+" ");
					}
					Console.WriteLine("validatemessage(b): ");
					foreach (byte b in validateMessage)
					{
						Console.Write(b + " ");
					}

					paramSet[commandType] = false;
					continue;
				}

				// Store information
				switch (commandType)
				{
					case (int)PARAM_NR.SIZE_X:
						{
							param.size_x = Convert.ToInt32(paramValue);
							break;
						}
					case (int)PARAM_NR.SIZE_Y:
						{
							param.size_y = Convert.ToInt32(paramValue);
							break;
						}
					case (int)PARAM_NR.SUBSECTIONS_X:
						{
							param.subsections_x = Convert.ToInt32(paramValue);
							break;
						}
					case (int)PARAM_NR.SUBSECTIONS_Y:
						{
							param.subsections_y = Convert.ToInt32(paramValue);
							break;
						}
					case (int)PARAM_NR.SEED:
						{
							param.seed = Convert.ToInt32(paramValue);
							break;
						}
					case (int)PARAM_NR.AMOUNT:
						{
							param.amount = Convert.ToInt32(paramValue);
							break;
						}
					case (int)PARAM_NR.DISCARDS:
						{
							param.discard = Convert.ToInt32(paramValue);
							break;
						}
				}
			}

			Console.WriteLine("All parameters set and done!");
			Console.WriteLine("size_x: {0}", param.size_x);
			Console.WriteLine("size_y: {0}", param.size_y);
			Console.WriteLine("subsections_x: {0}", param.subsections_x);
			Console.WriteLine("subsections_y: {0}", param.subsections_y);
			Console.WriteLine("seed: {0}", param.seed);
			Console.WriteLine("amount: {0}", param.amount);
			Console.WriteLine("discard: {0}", param.discard);
		}

		static void StartPerlinNoiseApp()
		{
			StringBuilder sb = new StringBuilder();
			sb.AppendFormat(" -x {0} -y {1} -sx {2} -sy {3} -s {4} -a {5} -d {6}", 
				param.size_x, param.size_y, param.subsections_x, param.subsections_y, 
				param.seed, param.amount, param.discard);
			string cmdArguments = sb.ToString();
			Console.WriteLine("Args: " + cmdArguments);

			calculation_timer.Start();
			Process myProcess = Process.Start("./perlinnoise/Perlin-Noise-Project.exe",
				Path.Combine(Path.GetPathRoot(Environment.SystemDirectory), cmdArguments));
			myProcess.WaitForExit();
			calculation_timer.Stop();

			Console.WriteLine("Time elapsed: " + calculation_timer.Elapsed);
		}

		static void SendFiles()
		{
			for (int i = 0; i < param.amount; i++)
			{
				// Determine mapPart
				int x = (param.discard + i) % param.subsections_x;
				int y = (int)((param.discard + i) / param.subsections_x);

				// Create filename and prepare message
				string filename = "x" + x + "y" + y + ".obj";
				byte[] message = new byte[P_SIZE];
				message[0] = (int)CMD_NR.FILENAME;
				int charIndex = 1;
				foreach (char c in filename)
				{
					message[charIndex] = Convert.ToByte(c);
					charIndex++;
				}

				

				// 1. Send filename
				SendMessage(message);

				// 2. Receive echo
				byte[] validateMessage = ReceiveMessage();

				// 3. Send validation or invalidation
				if (Enumerable.SequenceEqual(message, validateMessage))
				{
					SendMessage(message);
					Console.WriteLine("Filename transmitted!");
				}
				else 
				{
					SendMessage(new byte[P_SIZE]);
					Console.WriteLine("Filename could not be transmitted safely. Try again..");
					Environment.Exit(-1);
					continue;
				}

				// Create timing and prepare message
				message = new byte[P_SIZE];
				message[0] = (int)CMD_NR.TIMING;
				charIndex = 1;
				foreach (char c in (calculation_timer.Elapsed.ToString()))
				{
					message[charIndex] = Convert.ToByte(c);
					charIndex++;
				}


				// 4. Send elapsed Time
				SendMessage(message);

				// 5. Receive echo
				validateMessage = ReceiveMessage();

				// 6. Send validation
				if (Enumerable.SequenceEqual(message, validateMessage))
				{
					SendMessage(message);
					Console.WriteLine("Timing transmitted!");
				}
				else
				{
					SendMessage(new byte[P_SIZE]);
					Console.WriteLine("Timing could not be transmitted safely. Try again..");
					Environment.Exit(-1);
					continue;
				}


				// Transfer the file
				FileStream fileStream = System.IO.File.OpenRead("./Output/x" + x + "y" + y + ".obj");
				//FileStream fileStreamTest = System.IO.File.OpenWrite("./Output/TESTFILE.obj");
				while (true)
				{
					byte[] fileBuffer = new byte[P_SIZE];
					fileStream.Read(fileBuffer, 0, P_SIZE);
					SendMessage(fileBuffer);
					if (fileBuffer.Contains<byte>(0))
					{
						Console.WriteLine("EOF reached!");
						break;
					}
				}

				// Generate md5 Hash to validate the transmitted file
				fileStream.Position = 0;
				MD5 md5 = MD5.Create();
				string md5Hash = GetMd5HashFS(md5, fileStream);
				Console.WriteLine("Created Hash: "+md5Hash);

				byte[] md5HashByte = new byte[P_SIZE];
				
				for (int j = 0; j < md5Hash.Length; j++)
				{
					md5HashByte[j] = Convert.ToByte(md5Hash[j]);
				}
				// Send and validate md5 hash
				SendMessage(md5HashByte);
			}

		}

		static void Connect()
		{
			Console.WriteLine("Perlin-Noise-Project-Server");
			Console.WriteLine("Enter a Portnumber: ");
			port = Convert.ToInt32(Console.ReadLine());
			
			//port = 11000;
			Console.WriteLine("Port: " + port);
			Console.WriteLine("Wait for connection...");

			IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, port);
			Socket listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			listener.Bind(localEndPoint);
			listener.Listen(1);
			handler = listener.Accept();
		}

		static void SendMessage(byte[] answer)
		{
			try
			{
				handler.Send(answer);
			}
			catch (Exception e)
			{
				Console.WriteLine(errormessage);
				Environment.Exit(-1);
			}
		}

		static byte[] ReceiveMessage()
		{
			try
			{
				byte[] request = new byte[P_SIZE];
				int bytesRead = handler.Receive(request);
				int bytesToRead = P_SIZE;
				int offset = 0;

				if (bytesRead < P_SIZE)
				{
					while (bytesRead > 0)
					{
						bytesToRead -= bytesRead;
						offset += bytesRead;
						bytesRead = handler.Receive(request, offset, bytesToRead, SocketFlags.None);
					}
				}

				return request;
			}
			catch (Exception e)
			{
				Console.WriteLine(errormessage);
				Environment.Exit(-1);
				return new byte[1];
			}

		}

		static string GetMd5Hash(MD5 md5Hash, string input)
        {

            // Convert the input string to a byte array and compute the hash.
            byte[] data = md5Hash.ComputeHash(Encoding.UTF8.GetBytes(input));

            // Create a new Stringbuilder to collect the bytes
            // and create a string.
            StringBuilder sBuilder = new StringBuilder();

            // Loop through each byte of the hashed data
            // and format each one as a hexadecimal string.
            for (int i = 0; i < data.Length; i++)
            {
                sBuilder.Append(data[i].ToString("x2"));
            }

            // Return the hexadecimal string.
            return sBuilder.ToString();
        }

		static string GetMd5HashFS(MD5 md5Hash, FileStream input)
		{

			// Convert the input string to a byte array and compute the hash.
			byte[] data = md5Hash.ComputeHash(input);

			// Create a new Stringbuilder to collect the bytes
			// and create a string.
			StringBuilder sBuilder = new StringBuilder();

			// Loop through each byte of the hashed data
			// and format each one as a hexadecimal string.
			for (int i = 0; i < data.Length; i++)
			{
				sBuilder.Append(data[i].ToString("x2"));
			}

			// Return the hexadecimal string.
			return sBuilder.ToString();
		}

		// Verify a hash against a string.
		static bool VerifyMd5Hash(MD5 md5Hash, string input, string hash)
        {
            // Hash the input.
            string hashOfInput = GetMd5Hash(md5Hash, input);

            // Create a StringComparer an compare the hashes.
            StringComparer comparer = StringComparer.OrdinalIgnoreCase;

            if (0 == comparer.Compare(hashOfInput, hash))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

	}
}
