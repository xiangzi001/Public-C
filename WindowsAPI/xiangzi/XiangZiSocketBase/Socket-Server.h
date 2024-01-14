
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
        char*** pointer;

    };
    CommandList ComList;

    void Set(int ClientPos,int num, char* p) {
        //command[num] = c;
        ComList.pointer[ClientPos][num] = p;
    }

    bool** b_recv;
    bool is_recv(int ClientPos, int num) {
        if (b_recv[ClientPos][num]) {
            b_recv[ClientPos][num] = false;
            return true;
        }
        return false;
    }
    bool is_stop(int ClientPos) {
        return sAllClient[ClientPos];
    }
    void Recv(int ClientPos, int num) {
        b_recv[ClientPos][num] = true;
    }
    bool Send(int ClientPos, int num, int size) {
        int ret;
        SOCKET sClient = sAllClient[ClientPos];
        ret = send(sClient, (char*)(&num), 4, 0);
        ret = send(sClient, (char*)(&size), 4, 0);
        ret = send(sClient, ComList.pointer[ClientPos][num], size, 0);
        if (ret == SOCKET_ERROR) {
            int err = WSAGetLastError();

            if (err == WSAEWOULDBLOCK) {
                //Sleep(100);
            }
            else {
                closesocket(sClient);
                sIsFree[ClientPos] = true;
                return false;
            }
        }
        return true;
    }

    void InitCommand(int MaxCommand) {

        ComList.MaxCommand = MaxCommand;
        ComList.pointer = new char** [sMaxClient];
        for (int i = 0; i < sMaxClient; i++)
            ComList.pointer[i] = new char* [MaxCommand];

        b_recv = new bool* [sMaxClient];
        for (int i = 0; i < sMaxClient; i++)
            b_recv[i] = new bool[MaxCommand];
    }

    struct _DefClient{

        int typ = 0;
        int command = 0;
        int size = 0;
        char* p;

    };
    _DefClient* DefClient;

    void* _Start_cMain;
    int* _Start_cPos;
    void _ServerStart() {
        void* cMain = _Start_cMain;
        int* cPos = _Start_cPos;

        int addrlen = sizeof(sockaddr);

        int ret;
        int err;
        SOCKET sClient;
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
                        *cPos = i;
                        break;
                    }
                }
                thread mainClient((void(*)())(cMain));
                mainClient.detach();
            }
            else if (bStop) {
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
            else {
                for (int i = 0; i < sMaxClient; i = i + 1) {
                    if (sIsFree[i]){
                        continue;
                    }
                    else {
                        sClient = sAllClient[i];
                        switch (DefClient[i].typ)
                        {
                        case 0: {
                            ret = recv(sClient, (char*)(&(DefClient[i].command)), sizeof(int), 0);
                            break;
                        }
                        case 1: {
                            ret = recv(sClient, (char*)(&(DefClient[i].size)), sizeof(int), 0);
                            break;
                        }
                        default: {
                            ret = recv(sClient, DefClient[i].p, DefClient[i].size, 0);
                            break;
                        }
                        }

                        if (ret == SOCKET_ERROR) {
                            err = WSAGetLastError();
                            if (err == WSAEWOULDBLOCK) {
                                continue;
                            }
                            else if (err == WSAETIMEDOUT || err == WSAENETDOWN) {
                                closesocket(sClient);
                                sIsFree[i] = true;
                                continue;
                            }
                        }
                        else {
                            switch (DefClient[i].typ)
                            {
                            case 0: {
                                DefClient[i].typ = 1;
                                break;
                            }
                            case 1: {
                                DefClient[i].p = ComList.pointer[i][DefClient[i].command];
                                DefClient[i].typ = 2;
                                break;
                            }
                            default: {
                                Recv(i, DefClient[i].command);
                                DefClient[i].typ = 0;
                                break;
                            }
                            }
                        }
                    }
                }
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

            DefClient = new _DefClient[MaxClient];

        }

        void Start(void* ClientMain, int* ClientPos) {//监听  
            if (listen(sServer, sMaxClient)
                == SOCKET_ERROR)
            {
                closesocket(sServer);
                WSACleanup();
                return;
            }
            _Start_cMain = ClientMain;
            _Start_cPos = ClientPos;
            thread ServerMain(_ServerStart);
            ServerMain.detach();
        }
    };


}