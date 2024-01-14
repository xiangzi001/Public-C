
bool WaitSend(SOCKET s, const char* buffer, int n)
{
    int nLeft = n;  // ʣ����Ҫд����ֽ�����
    int idx = 0;    // �ѳɹ�д����ֽ�����
    int nwritten; // ÿ�ε���send()����д����ֽ�����
    while (nLeft > 0)
    {
        if ((nwritten = send(s, buffer + idx, nLeft, 0)) <= 0) return false;
        nLeft = nLeft - nwritten;
        idx = idx + nwritten;
    }
    return true;
}
