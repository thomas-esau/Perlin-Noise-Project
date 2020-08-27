using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using System.Security.Cryptography.X509Certificates;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using System.Reflection.Metadata;
using System.ComponentModel.Design;
using System.Threading.Tasks;
using System.Linq;
using System.IO;
using System.Security.Cryptography;
using System.Diagnostics;
using System.Net.WebSockets;
// State object for receiving data from remote device.  
public class StateObject
{
    public const int P_SIZE = 1024;
    // Client socket.  
    public Socket workSocket = null;
    // Size of receive buffer.  
    public const int BufferSize = P_SIZE;
    // Receive buffer.  
    public byte[] buffer = new byte[BufferSize];
    // Received data string.  
    public StringBuilder sb = new StringBuilder();
}

public class AsyncEventHandler
{
    // ManualResetEvent instances signal completion.  

    public ManualResetEvent sendDone =
        new ManualResetEvent(false);
    public ManualResetEvent receiveDone =
        new ManualResetEvent(false);
    public ManualResetEvent setParamsDone =
        new ManualResetEvent(false);
    public ManualResetEvent fileTransferDone =
        new ManualResetEvent(false);
}

public class ClientData
{
    public StateObject objState = new StateObject();
    public AsyncEventHandler socketState = new AsyncEventHandler();
    public String ipAddress = String.Empty;
    public int port = 0;
    public int discard = 0;
    public int amount = 0;
    public String timeElapsed = String.Empty;
}

public struct Parameter
{
    public int size_x, size_y, subsections_x, subsections_y, seed, amount, discard;
}

enum PARAM_NR
{
    // Parameter
    X, Y, SX, SY, S, A, D
}

enum CMD_NR
{
    // Other messages or commands
    DATA_VALID = 7, DATA_ERR, FILENAME, START_SENDING
}

public class AsynchronousClient
{
    public static int P_SIZE = 1024;
    public static Parameter param;
    public static ClientData currentClient;

    public static ManualResetEvent connectDone =
        new ManualResetEvent(false);

    public static ManualResetEvent allConnectionsEstablished =
        new ManualResetEvent(false);

    public static List<ClientData> clientList = new List<ClientData>();
    public static List<Socket> socketList = new List<Socket>();

    // The response from the remote device.  
    private static String response = String.Empty;

    private static void StartClient()
    {
        // Connect to a remote device.  
        try
        {
            param.size_x = 256;
            param.size_y = 256;
            param.subsections_x = 1;
            param.subsections_y = 1;
            param.seed = 2016;
            
            StartMenu();

            Connect();

            allConnectionsEstablished.WaitOne();

            ClientMenu();

            SendAllParams();

            ReceiveAllFiles();

            Console.ReadLine();
            Console.WriteLine("All Subsections has been created and transmitted.");

        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    private static void StartMenu()
    {
        //Menu
        bool menu = true;
        while (menu)
        {
            Console.WriteLine("Perlin-Noise-Project-Client: Menu");
            Console.WriteLine("\nCurrently loaded Settings: ");
            Console.WriteLine("Paramters:-x {0} -y {1} -sx {2} -sy {3} -s {4}",
                param.size_x, param.size_y, param.subsections_x, param.subsections_y, param.seed);
            Console.WriteLine("Map Parts: {0}", param.subsections_x * param.subsections_y);
            Console.WriteLine();
            Console.WriteLine("[1]: Change Parameters");
            Console.WriteLine("[2]: Initiating Connections to Servers");

            String input = Console.ReadLine();
            Console.WriteLine(input);

            switch (input)
            {
                case "1":
                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nEnter size_x value:");
                    try { param.size_x = Convert.ToInt32(Console.ReadLine()); } 
                    catch { Console.Clear(); Console.WriteLine("Invalid value.");  break; }

                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nEnter size_y value:");
                    try { param.size_y = Convert.ToInt32(Console.ReadLine()); }
                    catch { Console.Clear(); Console.WriteLine("Invalid value.");  break; }

                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nEnter subsections_x value:");
                    try { param.subsections_x = Convert.ToInt32(Console.ReadLine()); }
                    catch { Console.Clear(); Console.WriteLine("Invalid value.");  break; }

                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nEnter subsections_y value:");
                    try { param.subsections_y = Convert.ToInt32(Console.ReadLine()); }
                    catch { Console.Clear(); Console.WriteLine("Invalid value.");  break; }

                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nEnter seed:");
                    try { param.seed = Convert.ToInt32(Console.ReadLine()); }
                    catch { Console.Clear(); Console.WriteLine("Invalid value.");  break; }

                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nParameter are successfully updated.");

                    break;

                case "2":
                    Console.Clear();
                    Console.WriteLine("Begin Connecting to Servers.");
                    menu = false;
                    break;
                default:
                    Console.Clear();
                    Console.WriteLine("This Option is not available.");
                    break;
            }

        }
    }
    private static void ClientMenu()
    {
        int maxAmount = param.subsections_x * param.subsections_y;
        Console.Clear();
        Console.WriteLine("Paramters:-x {0} -y {1} -sx {2} -sy {3} -s {4}",
                param.size_x, param.size_y, param.subsections_x, param.subsections_y, param.seed);
        int sumAmount = 0;
        while (true)
        {

            //Console.Clear();
            Console.WriteLine("Server-List:");

            sumAmount = 0;
            foreach (ClientData s in clientList)
            {
                sumAmount += s.amount;
            }

            if (sumAmount == maxAmount)
            {
                foreach (ClientData s in clientList)
                {
                    Console.WriteLine("[{0}] {1}:{2} - Amount: {3}", clientList.IndexOf(s) + 1, s.ipAddress, s.port, s.amount);
                }
                Console.WriteLine("All Map Parts got allocated. Is this correct?\n[1]: Yes\n[2]: No");

                String input = Console.ReadLine();

                if (input == "1")
                {
                    List<Socket> noAssignment = new List<Socket>();
                    int discards = 0;
                    foreach (ClientData s in clientList)
                    {
                        if (s.amount == 0)
                            noAssignment.Add(s.objState.workSocket);
                        s.discard = discards;
                        discards += s.amount;
                    }
                    if (noAssignment.Count > 0)
                    {
                        Console.WriteLine("Server without allocated Map Parts were found. Disconnect...");
                        foreach (Socket s in noAssignment)
                        {
                            try
                            {
                                if (s == null) continue;
                                s.Shutdown(SocketShutdown.Both);
                                s.Close();
                                int socketIndex = socketList.IndexOf(s);
                                socketList.RemoveAt(socketIndex);
                                clientList.RemoveAt(socketIndex);
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine("Socket {0} could not be closed.", noAssignment.IndexOf(s) + 1);
                            }
                        }
                        
                    }
                    Console.WriteLine("Server Settings:");
                    foreach (ClientData s in clientList)
                    {
                        Console.WriteLine("[{0}]: Amount: {1}, Discards: {2}", clientList.IndexOf(s) + 1, s.amount, s.discard);
                    }

                    break;
                }
                else
                {
                    Console.Clear();
                    Console.WriteLine("Server-List:");
                }


            }
            foreach (ClientData s in clientList)
            {
                Console.WriteLine("[{0}] {1}:{2} - Amount: {3}", clientList.IndexOf(s) + 1, s.ipAddress, s.port, s.amount);
            }

            Console.WriteLine("maxAmount: {0}, sumAmount: {1} ", maxAmount, sumAmount);
            Console.WriteLine("Remaining Map Parts: {0} ", maxAmount - sumAmount);
            Console.WriteLine("Allocate the amount of Map Parts to the available Servers. Choose a Server: ");
            
            int serverNumber = Convert.ToInt32(Console.ReadLine());
            if (serverNumber > clientList.Count || serverNumber < 1)
            {
                Console.Clear();
                Console.WriteLine("Server Number out of Range.");
                continue;
            }

            Console.WriteLine("Amount of Map Parts: ");
            int inputAmount = Convert.ToInt32(Console.ReadLine());
            int temp = clientList[serverNumber - 1].amount;
            clientList[serverNumber - 1].amount = inputAmount;
            Console.Clear();
  
            sumAmount = 0;
            foreach (ClientData s in clientList)
            {
                sumAmount += s.amount;
            }
            if (sumAmount > maxAmount)
            {
                Console.Clear();
                Console.WriteLine("Maximum amount of Map Parts exceeded.");
                clientList[serverNumber - 1].amount = temp;
                continue;
            }
            
        }
    }
    private static void SendAllParams()
    {
        List<Thread> workerList = new List<Thread>();
        foreach (ClientData s in clientList)
        {
            Thread worker = new Thread(SendParamsTask);
            workerList.Add(worker);
            worker.Start(clientList.IndexOf(s));
        }

        foreach (ClientData s in clientList)
        {
            s.socketState.setParamsDone.WaitOne();
        }
        Console.WriteLine("All Clients are set and done!");
    }

    private static void SendParamsTask(Object obj)
    {
        Console.WriteLine("hey! ich bin obj: " + obj);
        Socket s = socketList[Convert.ToInt32(obj)];
        // Get ClientData
        int socketNumber = socketList.IndexOf(s);
        ClientData workingClient = clientList[socketNumber];
        Console.WriteLine(1);
        // Track state of sent parameters
        int paramLength = Enum.GetValues(typeof(PARAM_NR)).Length;
        bool[] paramSet = new bool[paramLength];

        // Check if all parameters are set.
        while (paramSet.All(x => x) != true)
        {
            foreach (int commandType in (PARAM_NR[])Enum.GetValues(typeof(PARAM_NR)))
            {
                if (paramSet[commandType] == true) continue;

                string paramValue = String.Empty;
                // Choose information to send
                switch (commandType)
                {
                    case (int)PARAM_NR.X:
                        {
                            paramValue = Convert.ToString(param.size_x);
                            break;
                        }
                    case (int)PARAM_NR.Y:
                        {
                            paramValue = Convert.ToString(param.size_y);
                            break;
                        }
                    case (int)PARAM_NR.SX:
                        {
                            paramValue = Convert.ToString(param.subsections_x);
                            break;
                        }
                    case (int)PARAM_NR.SY:
                        {
                            paramValue = Convert.ToString(param.subsections_y);
                            break;
                        }
                    case (int)PARAM_NR.S:
                        {
                            paramValue = Convert.ToString(param.seed);
                            break;
                        }
                    case (int)PARAM_NR.A:
                        {
                            paramValue = Convert.ToString(workingClient.amount);
                            break;
                        }
                    case (int)PARAM_NR.D:
                        {
                            paramValue = Convert.ToString(workingClient.discard);
                            break;
                        }
                }
                // 1. Send instructions to Server
                byte[] message = new byte[P_SIZE];
                message[0] = (byte) commandType;
                int charIndex = 1;
                    
                foreach (char c in paramValue)
                {
                    message[charIndex] = Convert.ToByte(c);
                    charIndex++;
                }
                Console.WriteLine(2);
                SendByte(s, message);
                Console.WriteLine(3);
                // 2. Receive echo for checking, if the correct vaules were transmitted.
                Receive(s);
                byte[] validateMessage = (byte[]) workingClient.objState.buffer.Clone();
                Console.WriteLine(4);
                // 3. Compare messages and send validation
                if (Enumerable.SequenceEqual(message, validateMessage))
                {
                    SendByte(s, message);
                    //workingClient.socketState.sendDone.Reset();
                    //Console.Read();
                    paramSet[commandType] = true;
                }
                else
                {
                    SendByte(s, new byte[P_SIZE]);
                    //Console.Read();
                    paramSet[commandType] = false;
                    continue;
                }
            }
        }

        Console.WriteLine("All parameters set and done!");

        // Successfully transmitted Params
        workingClient.socketState.setParamsDone.Set();
        }
    
    private static void ReceiveAllFiles()
    {
        List<Thread> workerList = new List<Thread>();
        foreach (ClientData s in clientList)
        {
            Thread worker = new Thread(ReceiveFileTask);
            workerList.Add(worker);
            worker.Start(clientList.IndexOf(s));
        }

        foreach (ClientData s in clientList)
        {
            s.socketState.fileTransferDone.WaitOne(); 
        }
        Console.WriteLine("All Clients have received their files!");
    }

    private static void ReceiveFileTask(Object obj)
    {
        Socket s = socketList[Convert.ToInt32(obj)];
        // Get ClientData
        int socketNumber = socketList.IndexOf(s);
        ClientData workingClient = clientList[socketNumber];

        // Get filename
        string filename = String.Empty;
        for (int i = 0; i < workingClient.amount; i++)
        {
            // 1. Receive filename
            Receive(s);
            byte[] message = (byte[])workingClient.objState.buffer.Clone();

            // 2. Send echo
            SendByte(s, message);

            // 3. Receive validation
            Receive(s);
            byte[] validateMessage = (byte[])workingClient.objState.buffer.Clone();

            int commandType = message[0];

            // 4. Receive ellapsed Time
            //Receive(s);
            //workingClient.socketState.receiveDone.WaitOne();
            //workingClient.socketState.receiveDone.Reset();
            //workingClient.timeElapsed = Encoding.ASCII.GetString(workingClient.objState.buffer);

            //Console.WriteLine("[{0}]: Ellapsed Time: {1}", clientList.IndexOf(workingClient) + 1, workingClient.timeElapsed);

            // Check if messages are equal
            if (Enumerable.SequenceEqual(message, validateMessage) && commandType == (int)CMD_NR.FILENAME)
            {
                Console.WriteLine("Filename transmitted successfully!");
                filename = Encoding.ASCII.GetString(message, 1, P_SIZE - 1);
                filename = filename.Split('\0')[0];
                Console.WriteLine("filename: {0}, length: {1}", filename, filename.Length);

            }
            else
            {
                Console.WriteLine("Filename could not be transmitted safely. Try again...");
                Environment.Exit(-1);
                i--;
                continue;
            }

            File.Delete(@"./Output/" + filename);
            FileStream fileStream = System.IO.File.OpenWrite("./Output/" + filename);
            byte[] empty = new byte[1024];
            while (true)
            {
                Receive(s);

                byte[] fileBuffer = workingClient.objState.buffer;
                    
                if (fileBuffer.Contains<byte>(0))
                {
                    Console.WriteLine("EOF reached!");
                        
                    List<byte> lastFileBuffer = fileBuffer.ToList<byte>();
                        
                    int lastElementIndex = lastFileBuffer.IndexOf(0);

                    fileBuffer = new byte[lastElementIndex];

                    for (int j = 0; j < lastElementIndex; j++)
                    {
                        fileBuffer[j] = lastFileBuffer[j];
                    }
                    fileStream.Write(fileBuffer, 0, lastElementIndex);
                    break;
                        
                }

                fileStream.Write(fileBuffer, 0, P_SIZE);
                    

            }

            // Generate md5 Hash to validate the transmitted file
            fileStream.Close();
            fileStream = System.IO.File.OpenRead("./Output/" + filename);
            MD5 md5 = MD5.Create();
            string md5Hash = GetMd5HashFS(md5, fileStream);
            fileStream.Close();

            // Receive and validate md5 hash
            Receive(s);

            StringBuilder sb = new StringBuilder();
            foreach (byte b in workingClient.objState.buffer)
            {
                if (b == 0) break;
                sb.Append(Convert.ToChar(b));
            }

            string validateMd5Hash = sb.ToString();
            Console.WriteLine("Created Hash: " + md5Hash);
            Console.WriteLine("Received Hash:" + validateMd5Hash);
            if (Enumerable.SequenceEqual(md5Hash, validateMd5Hash))
            {
                Console.WriteLine("MD5 sequence ok! File transfer successful.");
            }
            else
            {
                Console.WriteLine("Whoops! File transfer was not successful.");
            }
        }
        workingClient.socketState.fileTransferDone.Set();
    }
    private static void Connect()
    {
        Console.Clear();
        
        foreach (ClientData s in clientList)
        {
            Console.WriteLine("[{0}] {1}:{2}", clientList.IndexOf(s) + 1, s.ipAddress, s.port);
        }
        Console.WriteLine("[{0}] Enter an IP-Address: ", clientList.Count + 1);
        if (clientList.Count > 0) Console.WriteLine("If you are done with adding IP-Addresses, simply press \"Enter\".");
        String inputAddress = Console.ReadLine();

        if (inputAddress.Equals(""))
        {
            allConnectionsEstablished.Set();
            return;
        }

        Console.WriteLine("[{0}] Enter a Portnumber: ", clientList.Count + 1);
        int inputPort = Convert.ToInt32(Console.ReadLine());

        clientList.Add(new ClientData());
        currentClient = clientList[clientList.Count - 1];

        currentClient.ipAddress = inputAddress;
        currentClient.port = inputPort;

        //IPHostEntry ipHostInfo = Dns.GetHostEntry(inputAddress);
        //IPAddress ipAddress = ipHostInfo.AddressList[0];
        //IPEndPoint remoteEP = new IPEndPoint(ipAddress, inputPort);

        //Console.WriteLine("TEST: TO STRING: " + ipAddress.ToString());
        //Console.Read();
        // Create a TCP/IP socket.  
        
        Socket client = new Socket(AddressFamily.InterNetwork,
            SocketType.Stream, ProtocolType.Tcp);

        //client.BeginConnect(remoteEP, new AsyncCallback(ConnectCallback), client);
        client.BeginConnect(inputAddress, inputPort, new AsyncCallback(ConnectCallback), client);

        connectDone.WaitOne();
    }
    private static void ConnectCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the socket from the state object.  
            Socket client = (Socket)ar.AsyncState;

            // Add to Clientlist
            currentClient.objState.workSocket = client;
            socketList.Add(client);

            // Complete the connection.  
            client.EndConnect(ar);

            Console.WriteLine("Socket connected to {0}",
                client.RemoteEndPoint.ToString());

            // Signal that the connection has been made.  
            connectDone.Set();
            Connect();
            
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }
    private static void Receive(Socket client)
    {
        try
        {
            int socketIndex = socketList.IndexOf(client);
            ClientData currentClient = clientList[socketIndex];
            currentClient.objState = new StateObject();

            int bytesRead = client.Receive(currentClient.objState.buffer, 0, P_SIZE, SocketFlags.None);
            int bytesToRead = P_SIZE;
            int offset = 0;

            if (bytesRead < P_SIZE)
            {
                Console.WriteLine("Bytes Read: " + bytesRead);
                while (bytesRead > 0)
                {
                    bytesToRead -= bytesRead;
                    offset += bytesRead;
                    bytesRead = client.Receive(currentClient.objState.buffer, offset, bytesToRead, SocketFlags.None);
                    Console.WriteLine("     More Bytes: " + bytesRead);
                }
            }

        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    private static void CloseAllSockets()
    {
        foreach (Socket s in socketList)
        {
            s.Close(10);
        }
    }

    private static void SendByte(Socket client, byte[] byteData)
    {
        try 
        {
            client.Send(byteData);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
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

    public static int Main(String[] args)
    {
        StartClient();
        return 0;
    }
}