

class thread {
private:
public:
    void* func;

    void detach() {
        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, NULL, 0, NULL);
        CloseHandle(hThread);
    }

    thread(void* function) {
        func = function;
    }
};

class XThread {
private:
public:
    void* func;
    HANDLE hThread;
    bool close;
    va_list num;

    void detach(int Size, ...);

    void test(int Size, ...) {
        va_start(num, Size);
        for (int i = 0; i < Size; i++) {
            double k = va_arg(num, double);
            //cout << k << endl;
        }
        va_end(num);
    }

    XThread(void* function) {
        func = function;
        hThread = 0;
        close = true;
        num = 0;
    }
};

XThread* _XThread;
void _XThreadFunc() {
    XThread* Run = _XThread;

    (*(void(*)(va_list)) (Run->func))(Run->num);
    Run->close = true;

    va_end(Run->num);
}

void XThread::detach(int Size, ...) {
    va_start(num, Size);
    close = false;
    _XThread = &(*this);
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_XThreadFunc, NULL, 0, NULL);
    CloseHandle(hThread);
}