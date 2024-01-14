
namespace Client {
    sockaddr_in addrServ;
    SOCKET sServer;
    bool stop = true;

    struct CommandList {
        int MaxCommand;

        //int* command;
        char** pointer;

    };
    CommandList ComList;

    void Set(int num, char* p) {
        //command[num] = c;
        ComList.pointer[num] = p;
    }
    bool* b_recv;
    bool is_recv(int num) {
        if (b_recv[num]) {
            b_recv[num] = false;
            return true;
        }
        return false;
    }
    void Recv(int num) {
        b_recv[num] = true;
    }
    void Send(int num, int size) {
        int ret;
        //ret = send(sServer, (char*)(&num), 4, 0);
        //ret = send(sServer, (char*)(&size), 4, 0);
        //ret = send(sServer, ComList.pointer[num], size, 0);
        ret = send(sServer, (char*)(&num), 4, 0);
        ret = send(sServer, (char*)(&size), 4, 0);
        ret = send(sServer, ComList.pointer[num], size, 0);
        if (ret == SOCKET_ERROR) {
            int err = WSAGetLastError();

            if (err == WSAEWOULDBLOCK) {
                //Sleep(100);
            }
            else {
                closesocket(sServer);
                return;
            }
        }
    }

    void InitComList(int MaxCommand) {

        ComList.MaxCommand = MaxCommand;
        ComList.pointer = new char* [MaxCommand];
        b_recv = new bool[MaxCommand];
    }

    void DefClient() {
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
                ret = recv(sServer, (char*)(&command), sizeof(int), 0);
                break;
            }
            case 1: {
                ret = recv(sServer, (char*)(&size), sizeof(int), 0);
                break;
            }
            default: {
                ret = recv(sServer, p, size, 0);
                break;
            }
            }

            if (ret == SOCKET_ERROR) {
                err = WSAGetLastError();
                if (err == WSAEWOULDBLOCK) {
                }
                else if (err == WSAETIMEDOUT || err == WSAENETDOWN) {
                    closesocket(sServer);
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
                    Recv(command);
                    typ = 0;
                    break;
                }
                default:
                    break;
                }
            }

            if (stop)
                break;
        }
    }

    class XClient {
    private:
    public:
        void Init(const char* Address, int Port) {
            stop = true;

            WSADATA wsd;
            int ret;
            DWORD err;
            ret = WSAStartup(MAKEWORD(2, 2), &wsd);
            if (ret != 0)
            {
                err = WSAGetLastError();
                return ;
            }
            sServer = socket(AF_INET, SOCK_STREAM, 0);
            if (sServer == INVALID_SOCKET)
            {
                err = WSAGetLastError();
                return ;
            }

            addrServ.sin_addr.S_un.S_addr = inet_addr(Address);
            addrServ.sin_port = htons(Port);
            addrServ.sin_family = AF_INET;

        }


        void Start() {
            int ret = connect(sServer, (SOCKADDR*)&addrServ, sizeof(addrServ));
            if (ret != 0)
            {
            }
            stop = false;
            thread cMain(DefClient);
            cMain.detach();
        }

        void Close() {
            closesocket(sServer);
            stop = true;
        }
    };


}