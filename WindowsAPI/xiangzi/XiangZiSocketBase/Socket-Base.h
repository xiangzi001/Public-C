
bool WaitSend(SOCKET s, const char* buffer, int n)
{
    int nLeft = n;  // 剩余需要写入的字节数。
    int idx = 0;    // 已成功写入的字节数。
    int nwritten; // 每次调用send()函数写入的字节数。
    while (nLeft > 0)
    {
        if ((nwritten = send(s, buffer + idx, nLeft, 0)) <= 0) return false;
        nLeft = nLeft - nwritten;
        idx = idx + nwritten;
    }
    return true;
}
