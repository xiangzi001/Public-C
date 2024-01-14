
char _OpenFileName_filename[100];
OPENFILENAME _OpenFileName_ofn;
char* OpenFileName()
{//窗口形式选中文件路径
	//ShellExecute(NULL, "open", NULL, NULL, p_filename, SW_SHOWNORMAL);
	//char* p_filename = new char[100];
	ZeroMemory(&_OpenFileName_ofn, sizeof(_OpenFileName_ofn));
	ZeroMemory(_OpenFileName_filename, sizeof(_OpenFileName_filename));
	_OpenFileName_ofn.lStructSize = sizeof(_OpenFileName_ofn);
	_OpenFileName_ofn.hwndOwner = NULL;
	_OpenFileName_ofn.lpstrFile = (LPSTR)_OpenFileName_filename;
	_OpenFileName_ofn.lpstrFile[0] = '\0';
	_OpenFileName_ofn.nMaxFile = sizeof(_OpenFileName_filename);
	_OpenFileName_ofn.lpstrFilter = "All\0*.*\0 txt\0*.txt\0 jpg\0*.jpg\0 png\0*.png\0";
	_OpenFileName_ofn.nFilterIndex = 1;
	_OpenFileName_ofn.lpstrFileTitle = NULL;
	_OpenFileName_ofn.nMaxFileTitle = 0;
	_OpenFileName_ofn.lpstrInitialDir = NULL;
	_OpenFileName_ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	GetOpenFileName(&_OpenFileName_ofn);
	//delete[] p_filename;
	return _OpenFileName_ofn.lpstrFile;
}

#define Int			((int)		(4))
#define LongLong	((long long)(8))
#define Double 		((double)	(8))
#define Char		((char)		(1))

class XFile{
private:
	ifstream fr;
	ofstream fw;
public:
	char* byte;
	long long size;

	XFile() {
		byte = new char;
		size = 0;
	}
	void init() {
		byte = new char[size];
	}
	void init(long long s) {
		byte = new char[s];
		size = s;
	}

	void append(long long n) {
		char* byte_new = byte;
		byte = new char[size + n];
		for (int i = 0; i < size; i++)
			byte[i] = byte_new[i];
		delete byte_new;
		size = size + n;
	}
	void append(const char* n) {
		long long s = size;
		int len = strlen(n);
		append(len);
		set(s, n, len);
	}
	
	template<class T>
	void set(long long pos,T n) {
		int size = sizeof(n);
		char* m = (char*)(&n);
		for(int i=0; i<size; i++)
			byte[i+pos] = m[i];
	}
	template<class T>
	void set(long long pos, T* n, int m){
		for(int i=0; i<m; i++){
			set(pos, n[i]);
			pos = pos+sizeof(n[i]);
		}
	}
	
	template<class T>
	T value(long long pos,T typ){
		T n = *( (T*)(&(byte[pos])) );
		return n;
	}
	
	void reset(char* n, long long s) {
		byte = n;
		size = s;
	}
	void resize(const char* name) {
		fr.open(name);
		fr.seekg(0, ios::end);
		size = (int)fr.tellg();
		fr.close();
	}
	void read(const char* name) {
		fr.open(name, ios::binary);
		fr.read(byte,size);
		fr.close();
	}
	void write(const char* name) {
		fw.open(name);
		for (int i = 0; i < size; i++)
			fw << (byte[i]);
		fw.close();
	}

};

