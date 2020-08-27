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
    public Socket workSocket = null;
    public const int BufferSize = P_SIZE;
    public byte[] buffer = new byte[BufferSize];
    public int bytesToRead = StateObject.P_SIZE;
    public int offset = 0;
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
    public AsyncEventHandler asyncState = new AsyncEventHandler();
    public String ipAddress = String.Empty;
    public int port = 0;
    public int discards = 0;
    public int amount = 0;
    
    public Stopwatch transmission_timer = new Stopwatch();
    public TimeSpan calculation_time;
    
}

public struct CommandLineArguments
{
    public int size_x, size_y, subsections_x, subsections_y, seed;
}

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

public class AsynchronousClient
{
    public static int P_SIZE = StateObject.P_SIZE;
    public static CommandLineArguments commandLineArguments, standard_args;
    public static ClientData currentClient;

    public static ManualResetEvent connectDone =
        new ManualResetEvent(false);

    public static ManualResetEvent allConnectionsEstablished =
        new ManualResetEvent(false);

    public static List<ClientData> clientDataList = new List<ClientData>();
    public static List<Socket> socketList = new List<Socket>();

    // The response from the remote device.  
    private static String response = String.Empty;

    // Errormessage, that appears, when socketoperations failed.
    private static String errormessage = "A socketoperation caused an error. Terminate.";

    public static int Main(String[] args)
    {
        // Connect to a remote device.  
        try
        {

            Initialize();
            StartMenu();
            Connect();
            allConnectionsEstablished.WaitOne();
            ClientMenu();

            SendAllParams();
            ReceiveAllFiles();

            Console.WriteLine("All Subsections has been created and transmitted. Press \"Enter\", to print statistics.");
            Console.ReadLine();
            Console.Clear();
            PrintStatistics();
            Console.ReadLine();

        }
        catch (Exception e)
        {
            Console.WriteLine(errormessage);
            Environment.Exit(-1);
        }
        return 0;
    }

    static void Initialize()
    {
        commandLineArguments.size_x = 1024;
        commandLineArguments.size_y = 1024;
        commandLineArguments.subsections_x = 2;
        commandLineArguments.subsections_y = 2;
        commandLineArguments.seed = 2000;

        standard_args = commandLineArguments;
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
                commandLineArguments.size_x, commandLineArguments.size_y, commandLineArguments.subsections_x, commandLineArguments.subsections_y, commandLineArguments.seed);
            Console.WriteLine("Map Parts: {0}", commandLineArguments.subsections_x * commandLineArguments.subsections_y);
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
                    try { commandLineArguments.size_x = Convert.ToInt32(Console.ReadLine()); } 
                    catch 
                    {
                        commandLineArguments = standard_args;
                        Console.Clear(); Console.WriteLine("Invalid value.");
                        break; 
                    }

                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nEnter size_y value:");
                    try { commandLineArguments.size_y = Convert.ToInt32(Console.ReadLine()); }
                    catch { Console.Clear(); Console.WriteLine("Invalid value.");  break; }

                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nEnter subsections_x value (Must divide size_x without remainder): ");
                    try 
                    { 
                        commandLineArguments.subsections_x = Convert.ToInt32(Console.ReadLine()); 
                        if (commandLineArguments.size_x % commandLineArguments.subsections_x != 0)
                        {
                            Console.Clear(); Console.WriteLine("Invalid value: Division with remainder.");
                            commandLineArguments = standard_args;
                            break;
                        }
                    }
                    catch 
                    {
                        commandLineArguments = standard_args;
                        Console.Clear(); Console.WriteLine("Invalid value.");  
                        break; 
                    }

                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nEnter subsections_y value: (Must divide size_y without remainder): ");
                    try { 
                        commandLineArguments.subsections_y = Convert.ToInt32(Console.ReadLine());
                        if (commandLineArguments.size_y % commandLineArguments.subsections_y != 0)
                        {
                            Console.Clear(); Console.WriteLine("Invalid value: Division with remainder.");
                            commandLineArguments = standard_args;
                            break;
                        }
                    }
                    catch 
                    {
                        commandLineArguments = standard_args;
                        Console.Clear(); Console.WriteLine("Invalid value.");
                        break; 
                    }

                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nEnter seed:");
                    try { commandLineArguments.seed = Convert.ToInt32(Console.ReadLine()); }
                    catch 
                    {
                        commandLineArguments = standard_args;
                        Console.Clear(); Console.WriteLine("Invalid value.");  
                        break; 
                    }

                    Console.Clear();
                    Console.WriteLine("Parameter Settings:\nParameters got successfully updated.");

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
        int maxAmount = commandLineArguments.subsections_x * commandLineArguments.subsections_y;
        Console.Clear();
        Console.WriteLine("Paramters:-x {0} -y {1} -sx {2} -sy {3} -s {4}",
                commandLineArguments.size_x, commandLineArguments.size_y, commandLineArguments.subsections_x, commandLineArguments.subsections_y, commandLineArguments.seed);
        int sumAmount = 0;
        while (true)
        {

            //Console.Clear();
            Console.WriteLine("Server-List:");

            sumAmount = 0;
            foreach (ClientData s in clientDataList)
            {
                sumAmount += s.amount;
            }

            if (sumAmount == maxAmount)
            {
                foreach (ClientData s in clientDataList)
                {
                    Console.WriteLine("[{0}] {1}:{2} - Amount: {3}", clientDataList.IndexOf(s) + 1, s.ipAddress, s.port, s.amount);
                }
                Console.WriteLine("All Map Parts got allocated. Is this correct?\n[1]: Yes\n[2]: No");

                String input = Console.ReadLine();

                if (input == "1")
                {
                    List<Socket> noAssignment = new List<Socket>();
                    int discards = 0;
                    foreach (ClientData s in clientDataList)
                    {
                        if (s.amount == 0)
                            noAssignment.Add(s.objState.workSocket);
                        s.discards = discards;
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
                                clientDataList.RemoveAt(socketIndex);
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine("Socket {0} could not be closed.", noAssignment.IndexOf(s) + 1);
                                Environment.Exit(-1);
                            }
                        }
                        
                    }
                    Console.WriteLine("Server Settings:");
                    foreach (ClientData s in clientDataList)
                    {
                        Console.WriteLine("[{0}]: Amount: {1}, Discards: {2}", clientDataList.IndexOf(s) + 1, s.amount, s.discards);
                    }

                    break;
                }
                else
                {
                    Console.Clear();
                    Console.WriteLine("Server-List:");
                }


            }
            foreach (ClientData s in clientDataList)
            {
                Console.WriteLine("[{0}] {1}:{2} - Amount: {3}", clientDataList.IndexOf(s) + 1, s.ipAddress, s.port, s.amount);
            }

            Console.WriteLine("maxAmount: {0}, sumAmount: {1} ", maxAmount, sumAmount);
            Console.WriteLine("Remaining Map Parts: {0} ", maxAmount - sumAmount);
            Console.WriteLine("Allocate the amount of Map Parts to the available Servers. Choose a Server: ");
            
            int serverNumber = Convert.ToInt32(Console.ReadLine());
            if (serverNumber > clientDataList.Count || serverNumber < 1)
            {
                Console.Clear();
                Console.WriteLine("Server Number out of Range.");
                continue;
            }

            Console.WriteLine("Amount of Map Parts: ");
            int inputAmount = Convert.ToInt32(Console.ReadLine());
            int temp = clientDataList[serverNumber - 1].amount;
            clientDataList[serverNumber - 1].amount = inputAmount;
            Console.Clear();
  
            sumAmount = 0;
            foreach (ClientData s in clientDataList)
            {
                sumAmount += s.amount;
            }
            if (sumAmount > maxAmount)
            {
                Console.Clear();
                Console.WriteLine("Maximum amount of Map Parts exceeded.");
                clientDataList[serverNumber - 1].amount = temp;
                continue;
            }
            
        }
    }
    private static void SendAllParams()
    {

        foreach (Socket s in socketList)
        {
            SendParamsAsync(s);
        }

        foreach (ClientData s in clientDataList)
        {
            s.asyncState.setParamsDone.WaitOne();
        }
        Console.WriteLine("All clientsockets are set and done!");
    }
    private static void PrintStatistics()
    {
        Console.WriteLine("Calculation timings: ");
        TimeSpan totalCalc = new TimeSpan(0, 0, 0, 0, 0);
        foreach (ClientData client in clientDataList)
        {
            Console.WriteLine("[{0}]: {1}", clientDataList.IndexOf(client) + 1, client.calculation_time.ToString());
            if (TimeSpan.Compare(totalCalc, client.calculation_time) == -1)
                totalCalc = client.calculation_time;
        }
        Console.WriteLine("Total calculation time: " + totalCalc.ToString());
        Console.WriteLine();

        Console.WriteLine("Calculation + transmission timings: ");
        TimeSpan totalCalcTrans = new TimeSpan(0, 0, 0, 0, 0);
        foreach (ClientData client in clientDataList)
        {
            Console.WriteLine("[{0}]: {1}", clientDataList.IndexOf(client) + 1, client.transmission_timer.Elapsed.ToString());
            if (TimeSpan.Compare(totalCalcTrans, client.transmission_timer.Elapsed) == -1)
                totalCalcTrans = client.transmission_timer.Elapsed;
        }
        Console.WriteLine("Total calculation + transmission time: " + totalCalcTrans.ToString());
    }
    private static async void SendParamsAsync(Socket s)
    {
        await (Task.Run(() =>
        {
            // Get ClientData
            int socketNumber = socketList.IndexOf(s);
            ClientData workingClient = clientDataList[socketNumber];

            workingClient.transmission_timer.Start();

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
                        case (int)PARAM_NR.SIZE_X:
                            {
                                paramValue = Convert.ToString(commandLineArguments.size_x);
                                break;
                            }
                        case (int)PARAM_NR.SIZE_Y:
                            {
                                paramValue = Convert.ToString(commandLineArguments.size_y);
                                break;
                            }
                        case (int)PARAM_NR.SUBSECTIONS_X:
                            {
                                paramValue = Convert.ToString(commandLineArguments.subsections_x);
                                break;
                            }
                        case (int)PARAM_NR.SUBSECTIONS_Y:
                            {
                                paramValue = Convert.ToString(commandLineArguments.subsections_y);
                                break;
                            }
                        case (int)PARAM_NR.SEED:
                            {
                                paramValue = Convert.ToString(commandLineArguments.seed);
                                break;
                            }
                        case (int)PARAM_NR.AMOUNT:
                            {
                                paramValue = Convert.ToString(workingClient.amount);
                                break;
                            }
                        case (int)PARAM_NR.DISCARDS:
                            {
                                paramValue = Convert.ToString(workingClient.discards);
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
                    Send(s, message);
                    workingClient.asyncState.sendDone.WaitOne();
                    workingClient.asyncState.sendDone.Reset();

                    // 2. Receive echo for checking, if the correct vaules were transmitted.
                    Receive(s);
                    workingClient.asyncState.receiveDone.WaitOne();
                    workingClient.asyncState.receiveDone.Reset();
                    byte[] validateMessage = (byte[]) workingClient.objState.buffer.Clone();
                    // 3. Compare messages and send validation
                    if (Enumerable.SequenceEqual(message, validateMessage))
                    {
                        Send(s, message);
                        workingClient.asyncState.sendDone.WaitOne();
                        workingClient.asyncState.sendDone.Reset();
                        //Console.Read();
                        paramSet[commandType] = true;
                    }
                    else
                    {
                        Send(s, new byte[P_SIZE]);
                        //Console.Read();
                        workingClient.asyncState.sendDone.WaitOne();
                        paramSet[commandType] = false;
                        continue;
                    }
                }
            }

            Console.WriteLine("All parameters set and done!");

            // Successfully transmitted Params
            workingClient.asyncState.setParamsDone.Set();
        }
        ));

    }
    private static void ReceiveAllFiles()
    {
        foreach (Socket s in socketList)
        {
            ReceiveFilesAsync(s);
        }

        foreach (ClientData s in clientDataList)
        {
            s.asyncState.fileTransferDone.WaitOne();
        }
        Console.WriteLine("All clientsockets have received their files!");
    }
    private static async void ReceiveFilesAsync(Socket s)
    {
        await (Task.Run(() =>
        {
            // Get ClientData
            int socketNumber = socketList.IndexOf(s);
            ClientData workingClient = clientDataList[socketNumber];

            // Get filename
            string filename = String.Empty;
            for (int i = 0; i < workingClient.amount; i++)
            {

                // 1. Receive filename
                Receive(s);
                workingClient.asyncState.receiveDone.WaitOne();
                workingClient.asyncState.receiveDone.Reset();
                byte[] message = (byte[])workingClient.objState.buffer.Clone();

                // 2. Send echo
                Send(s, message);
                workingClient.asyncState.sendDone.WaitOne();
                workingClient.asyncState.sendDone.Reset();

                // 3. Receive validation
                Receive(s);
                workingClient.asyncState.receiveDone.WaitOne();
                workingClient.asyncState.receiveDone.Reset();
                byte[] validateMessage = (byte[])workingClient.objState.buffer.Clone();

                int commandType = message[0];

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
                    continue;
                }


                // 4. Receive ellapsed Time
                Receive(s);
                workingClient.asyncState.receiveDone.WaitOne();
                workingClient.asyncState.receiveDone.Reset();
                message = (byte[])workingClient.objState.buffer.Clone();
                

                // 5. Send echo
                Send(s, message);
                workingClient.asyncState.sendDone.WaitOne();
                workingClient.asyncState.sendDone.Reset();

                // 6. Receive validation
                Receive(s);
                workingClient.asyncState.receiveDone.WaitOne();
                workingClient.asyncState.receiveDone.Reset();
                validateMessage = (byte[])workingClient.objState.buffer.Clone();

                commandType = message[0];

                String ellapsedTime;
                // Check if messages are equal
                if (Enumerable.SequenceEqual(message, validateMessage) && commandType == (int)CMD_NR.TIMING)
                {
                    Console.WriteLine("Timing transmitted successfully!");
                    ellapsedTime = Encoding.ASCII.GetString(message, 1, P_SIZE - 1).Trim((char)0);
                    workingClient.calculation_time = TimeSpan.Parse(ellapsedTime);
                }
                else
                {
                    Console.WriteLine("Timing could not be transmitted safely. Try again...");
                    Environment.Exit(-1);
                    continue;
                }




                File.Delete(@"./Output/" + filename);
                FileStream fileStream = System.IO.File.OpenWrite("./Output/" + filename);
                byte[] empty = new byte[P_SIZE];
                while (true)
                {
                    Receive(s);
                    workingClient.asyncState.receiveDone.WaitOne();
                    workingClient.asyncState.receiveDone.Reset();

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
                workingClient.asyncState.receiveDone.WaitOne();
                workingClient.asyncState.receiveDone.Reset();

                StringBuilder sb = new StringBuilder();
                foreach (byte b in workingClient.objState.buffer)
                {
                    if (b == 0) break;
                    sb.Append(Convert.ToChar(b));
                }

                workingClient.transmission_timer.Stop();

                string validateMd5Hash = sb.ToString();
                Console.WriteLine("Created Hash:  " + md5Hash);
                Console.WriteLine("Received Hash: " + validateMd5Hash);

                Console.WriteLine("Calculation time: " + ellapsedTime.ToString());
                Console.WriteLine("Total time: " + workingClient.transmission_timer.Elapsed.ToString());

                if (Enumerable.SequenceEqual(md5Hash, validateMd5Hash))
                {
                    Console.WriteLine("MD5 sequence ok! File transfer successful.");
                }
                else
                {
                    Console.WriteLine("Whoops! File transfer was not successful.");
                    Environment.Exit(-1);
                }
            }
            workingClient.asyncState.fileTransferDone.Set();
        }));
    }
    private static void Connect()
    {
        Console.Clear();
        
        foreach (ClientData s in clientDataList)
        {
            Console.WriteLine("[{0}] {1}:{2}", clientDataList.IndexOf(s) + 1, s.ipAddress, s.port);
        }
        Console.WriteLine("[{0}] Enter an IP-Address: ", clientDataList.Count + 1);

        if (clientDataList.Count > 0) Console.WriteLine("If you are done with adding IP-Addresses, simply press \"Enter\".");
        String inputAddress = Console.ReadLine();

        if (inputAddress.Equals(""))
        {
            allConnectionsEstablished.Set();
            return;
        }

        Console.WriteLine("[{0}] Enter a Portnumber: ", clientDataList.Count + 1);
        int inputPort = Convert.ToInt32(Console.ReadLine());

        clientDataList.Add(new ClientData());
        currentClient = clientDataList[clientDataList.Count - 1];

        currentClient.ipAddress = inputAddress;
        currentClient.port = inputPort;
        
        Socket client = new Socket(AddressFamily.InterNetwork,
            SocketType.Stream, ProtocolType.Tcp);

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
            Console.WriteLine(errormessage);
            Environment.Exit(-1);
        }
    }
    private static void Receive(Socket client)
    {
        try
        {
            // Create the state object.  
            int socketIndex = socketList.IndexOf(client);
            ClientData currentClient = clientDataList[socketIndex];
            currentClient.objState = new StateObject();
            currentClient.objState.workSocket = client;

            // Begin receiving the data from the remote device.  
            client.BeginReceive(currentClient.objState.buffer, currentClient.objState.offset, StateObject.BufferSize, SocketFlags.None,
                new AsyncCallback(ReceiveCallback), currentClient.objState);
        }
        catch (Exception e)
        {
            Console.WriteLine(errormessage);
            Environment.Exit(-1);
        }
    }
    private static void CloseAllSockets()
    {
        foreach (Socket s in socketList)
        {
            s.Close(10);
        }
    }

    private static void ReceiveCallback(IAsyncResult ar)
    {
        try
        {
            StateObject state = (StateObject)ar.AsyncState;
            Socket client = state.workSocket;

            // Read data from the remote device.  
            int bytesRead = client.EndReceive(ar);
            
            state.bytesToRead -= bytesRead;
            state.offset += bytesRead;
            if (state.bytesToRead > 0)                                      
            {
                // Get the rest of the data.  
                client.BeginReceive(state.buffer, state.offset, state.bytesToRead , 0,
                    new AsyncCallback(ReceiveCallback), state);
            }
            else
            {
                // Signal that all bytes have been received.  
                state.bytesToRead = P_SIZE;
                state.offset = 0;
                int socketIndex = socketList.IndexOf(client);
                clientDataList[socketIndex].asyncState.receiveDone.Set();
            }
        }
        catch (Exception e)
        {
            Console.WriteLine(errormessage);
            Environment.Exit(-1);
        }
    }
    private static void Send(Socket client, byte[] byteData)
    {
        // Begin sending the data to the remote device.  
        client.BeginSend(byteData, 0, byteData.Length, SocketFlags.None,
            new AsyncCallback(SendCallback), client);
    }
    private static void SendCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the socket from the state object.  
            Socket client = (Socket)ar.AsyncState;

            // Complete sending the data to the remote device.  
            int bytesSent = client.EndSend(ar);

            // Signal that all bytes have been sent.  
            int socketIndex = socketList.IndexOf(client);
            clientDataList[socketIndex].asyncState.sendDone.Set();

        }
        catch (Exception e)
        {
            Console.WriteLine(errormessage);
            Environment.Exit(-1);
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




}