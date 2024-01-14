
namespace Server {
    sockaddr_in addrServ;
    SOCKET sServer;
    bool bStop;

    SOCKET* sAllClient;
    bool* sIsFree;
    int sMaxClient = 0;


    class CommandList {
    public:
        int MaxCommand;
        //int* command;
        char** pointer;

    };
    CommandList ComList;

    void Set(int num, char* p) {
        //command[num] = c;
        ComList.pointer[num] = p;
    }

    bool** b_recv;
    bool is_recv(int c, int num) {
        if (b_recv[c][num]) {
            b_recv[c][num] = false;
            return true;
        }
        return false;
    }
    void Recv(int c, int num) {
        b_recv[c][num] = true;
    }
    void Send(int c, int num, int size) {
        int ret;
        SOCKET sClient = sAllClient[c];
        ret = send(sClient, (char*)(&num), 4, 0);
        ret = send(sClient, (char*)(&size), 4, 0);
        ret = send(sClient, ComList.pointer[num], size, 0);
        if (ret == SOCKET_ERROR) {
            int err = WSAGetLastError();

            if (err == WSAEWOULDBLOCK) {
                //Sleep(100);
            }
            else {
                closesocket(sClient);
                sIsFree[c] = true;
                return;
            }
        }
    }

    void InitCommand(int MaxCommand) {

        ComList.MaxCommand = MaxCommand;
        ComList.pointer = new char* [MaxCommand];

        b_recv = new bool* [sMaxClient];
        for (int i = 0; i < sMaxClient; i++)
            b_recv[i] = new bool[MaxCommand];
    }

    int _Client_sClientPos;
    void DefClient() {
        int sPos = _Client_sClientPos;
        SOCKET sClient = sAllClient[sPos];
        int ret = 0;
        int err = 0;

        int typ = 0;
        int command = 0;
        int size = 0;
        char* p;

        while (true) {
            switch (typ)
            {
            case 0: {
                ret = recv(sClient, (char*)(&command), sizeof(int), 0);
                break;
            }
            case 1: {
                ret = recv(sClient, (char*)(&size), sizeof(int), 0);
                break;
            }
            default: {
                ret = recv(sClient, p, size, 0);
                break;
            }
            }

            if (ret == SOCKET_ERROR) {
                err = WSAGetLastError();
                if (err == WSAEWOULDBLOCK) {
                }
                else if (err == WSAETIMEDOUT || err == WSAENETDOWN) {
                    closesocket(sClient);
                    sIsFree[sPos] = true;
                    return;
                }
            }
            else {
                switch (typ)
                {
                case 0: {
                    typ = 1;
                    break;
                }
                case 1: {
                    p = ComList.pointer[command];
                    ZeroMemory(p, size);
                    typ = 2;
                    break;
                }
                case 2: {
                    Recv(sPos, command);
                    typ = 0;
                    break;
                }
                default:
                    break;
                }
            }
            ///*
            //*/
        }
    }

    void* _Start_cMain;
    int* _Start_cPos;
    void _ServerStart() {
        void* cMain = _Start_cMain;
        int* cPos = _Start_cPos;

        int addrlen = sizeof(sockaddr);

        //循环
        while (true)
        {
            SOCKET sConnet;
            sConnet = accept(sServer, (sockaddr*)&addrServ, &addrlen);
            if (sConnet != INVALID_SOCKET)
            {
                for (int i = 0; i < sMaxClient; i++) {
                    if (sIsFree[i]) {
                        sIsFree[i] = false;
                        sAllClient[i] = sConnet;
                        _Client_sClientPos = i;
                        *cPos = i;
                        break;
                    }
                }
                thread tClient(DefClient);
                tClient.detach();
                thread mainClient((void(*)())(cMain));
                mainClient.detach();
            }
            if (bStop) {
                //释放服务器Socket
                for (int i = 0; i < sMaxClient; i++) {
                    if (!(sIsFree[i])) {
                        closesocket(sAllClient[i]);
                    }
                }
                closesocket(sServer);
                WSACleanup();
                break;
            }
        }
    }

    class XServer {
    private:
    public:
        void InitServer(const char* Address, int Port, int MaxClient) {
            WSADATA wsd;

            //设置服务器Socket地址
            addrServ.sin_family = AF_INET;
            addrServ.sin_addr.S_un.S_addr = inet_addr(Address);
            addrServ.sin_port = htons(Port);

            //初始化Socket  
            if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
            {
                return ;
            }
            //创建用于监听的Socket  
            sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (INVALID_SOCKET == sServer)
            {
                WSACleanup();
                return ;
            }
            //设置Socket为非阻塞模式  
            int iMode = 1;
            if (ioctlsocket(sServer, FIONBIO, (u_long FAR*) & iMode)
                == SOCKET_ERROR)
            {
                WSACleanup();
                return ;
            }

            //绑定Socket Server到本地地址  
            if (bind(sServer, (const struct sockaddr*)&addrServ, sizeof(sockaddr_in))
                == SOCKET_ERROR)
            {
                closesocket(sServer);
                WSACleanup();
                return ;
            }

            //初始化
            sMaxClient = MaxClient;
            sAllClient = new SOCKET[MaxClient];

            sIsFree = new bool[MaxClient];
            for (int i = 0; i < MaxClient; i++)
                sIsFree[i] = true;

        }

        void Start(void* cMain, int* cPos) {//监听  
            if (listen(sServer, sMaxClient)
                == SOCKET_ERROR)
            {
                closesocket(sServer);
                WSACleanup();
                return;
            }
            _Start_cMain = cMain;
            _Start_cPos = cPos;
            thread ServerMain(_ServerStart);
            ServerMain.detach();
        }
    };


}