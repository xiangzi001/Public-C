
enum EVD_type {
	_QR,
	_Jacobi
};

template<typename T>
void EigenValue(MAT<T>& A, MAT<T>& Q, T eps) {
	bool flag = true;

	T* ur = new T[A.rows];
	MAT<T> C(A.rows, A.rows);
	T* Temp;

	while (flag) {

		QR_HouseHolder(Q, A, ur);
		times(A, Q, C);

		Temp = A.data;
		A.data = C.data;
		C.data = Temp;
		flag = false;
		for (int i = 2; i < A.rows; i++) {
			for (int j = 0; j < i - 1; j++)
				if (fabs(A[i][j]) > eps) {
					flag = true;
					break;
				}
		}
	}
	delete[] ur;
}


template<typename T>
void HouseHolder_EVD(MAT<T>& Q, MAT<T>& R, T* ur) {
	int i, j, k, r;
	T temp, sum, cr, hr;
	for (r = 0; r < R.rows - 1; r++) {
		temp = 0;
		for (k = r; k < R.rows; k++)
			temp += fabs(R[k][r]);
		if (temp > 0)
		{
			sum = 0;
			for (k = r + 1; k < R.rows; k++)
				sum += R[k][r] * R[k][r];
			cr = sqrt(sum);
			if (R[r + 1][r] > 0.0)cr = -cr;
			hr = cr * (cr - R[r + 1][r]);

			//定义ur
			ur[r + 1] = R[r + 1][r] - cr;
			for (i = r + 2; i < R.rows; i++)
			{
				ur[i] = R[i][r];
			}

			//定义Q
			for (i = 0; i < R.rows; i++)
			{
				sum = 0;
				for (j = r + 1; j < R.rows; j++)
					sum += Q[i][j] * ur[j];
				sum = sum / hr;
				for (j = r + 1; j < R.cols; j++)
				{
					Q[i][j] = Q[i][j] - ur[j] * sum;
				}
			}

			//定义R
			for (i = r; i < R.rows; i++)
			{
				sum = 0;
				for (j = r + 1; j < R.rows; j++)
					sum += R[i][j] * ur[j];
				sum = sum / hr;
				for (j = r + 1; j < R.cols; j++)
				{
					R[i][j] = R[i][j] - ur[j] * sum;
				}
			}
			for (j = r; j < R.cols; j++)
			{
				sum = 0;
				for (i = r + 1; i < R.rows; i++)
					sum += R[i][j] * ur[i];
				sum = sum / hr;
				for (i = r + 1; i < R.rows; i++)
					R[i][j] = R[i][j] - ur[i] * sum;
			}
		}
	}
}

template<typename T>
void Givens_EVD(MAT<T>& Q, MAT<T>& R, int m, int n, T eps, T* Temp) {

	while (n - m - 1) {

		if (fabs(R[m + 1][m]) < eps) {
			m++;
			continue;
		}

		double theta = atan2(R[m + 1][m], R[m][m]);
		T c = cos(theta);
		T s = sin(theta);
		for (int k = m; k < n; k++) {
			T temp = c * R[m][k] + s * R[m + 1][k];
			R[m + 1][k] = -s * R[m][k] + c * R[m + 1][k];
			R[m][k] = temp;
		}
		for (int k = m; k < n; k++) {
			T temp = c * R[k][m] + s * R[k][m + 1];
			R[k][m + 1] = -s * R[k][m] + c * R[k][m + 1];
			R[k][m] = temp;
		}
		for (int k = 0; k < R.rows; k++) {
			T temp = c * Q[k][m] + s * Q[k][m + 1];
			Q[k][m + 1] = -s * Q[k][m] + c * Q[k][m + 1];
			Q[k][m] = temp;
		}

		for (int i = m + 2; i < n; i++) {
			if (fabs(R[i][i - 1]) < eps) {
				if (n - i - 1) {
					Givens_EVD(Q, R, m, i, eps, Temp);
					m = i;
					break;
				}
				else {
					n--;
				}
			}
			else {
				theta = atan2(R[i][i - 2], R[i - 1][i - 2]);
				c = cos(theta);
				s = sin(theta);

				for (int k = m; k < n; k++) {
					T temp = c * R[i - 1][k] + s * R[i][k];
					R[i][k] = -s * R[i - 1][k] + c * R[i][k];
					R[i - 1][k] = temp;
				}
				for (int k = m; k < n; k++) {
					T temp = c * R[k][i - 1] + s * R[k][i];
					R[k][i] = -s * R[k][i - 1] + c * R[k][i];
					R[k][i - 1] = temp;
				}

				for (int k = 0; k < R.rows; k++) {
					T temp = c * Q[k][i - 1] + s * Q[k][i];
					Q[k][i] = -s * Q[k][i - 1] + c * Q[k][i];
					Q[k][i - 1] = temp;
				}
			}
		}
		for (int i = m; i < n; i++) {
			Temp[i] = Temp[i] + R[n - 1][n - 1];
			R[i][i] = R[i][i] - R[n - 1][n - 1];
		}
	}
}

template<typename T>
void EVD_Q_QR(MAT<T>& A, MAT<T>& Q, T eps) {
	int i = 0;
	for (i = 0; i < Q.rows * Q.cols; i++) {
		Q.data[i] = 0;
	}
	for (i = 0; i < Q.rows; i++) {
		Q[i][i] = 1;
	}

	T* ur = new T[A.rows];
	HouseHolder_EVD(Q, A, ur);

	for (int i = 0; i < A.rows; i++) {
		ur[i] = 0;
	}
	Givens_EVD(Q, A, 0, A.rows, eps, ur);

	for (int i = 0; i < A.rows; i++) {
		A[i][i] = A[i][i] + ur[i];
	}

	for (int i = 0; i < A.rows; i++) {
		T max = 0;
		int c = i;
		T value;
		for (int j = i; j < A.rows; j++) {
			if (A[j][j] > max) {
				max = A[j][j];
				c = j;
			}
		}
		if (c != i) {
			value = A[c][c];
			A[c][c] = A[i][i];
			A[i][i] = value;
			for (int j = 0; j < A.rows; j++) {
				value = Q[c][j];
				Q[c][j] = Q[i][j];
				Q[i][j] = value;
			}
		}
	}
	delete[] ur;
}

template <typename T>
void EVD_Q_Jacobi(MAT<T>& A, MAT<T>& Q, T dbEps)
{
	// 第一步：初始化特征向量矩阵pbdEigenValues即V
	for (int i = 0; i < Q.rows; i++)
	{
		// 对角线为1，其他为0
		Q[i][i] = 1.0f;
		for (int j = 0; j < Q.rows; j++)
		{
			if (i != j)
				Q[i][j] = 0.0f;
		}
	}

	while (1)
	{
		// 第二步：对矩阵A.data即矩阵A，求出最大索引nRow、nCol即p、q
		T dbMax = A.data[1];
		int nRow = 0;
		int nCol = 1; // 因为求的是非对角线上最大值，所有从1开始
		for (int i = 1; i < A.rows; i++)			//行
		{
			for (int j = 0; j < i; j++)		//列
			{
				T d = fabs(A[i][j]);

				if (d > dbMax)
				{
					dbMax = d;
					nRow = i;
					nCol = j;
				}
			}
		}

		if (dbMax < dbEps)
			break;

		T dbApp = A[nRow][nRow];
		T dbApq = A[nRow][nCol];
		T dbAqq = A[nCol][nCol];

		T dbAngle = 0.5 * atan2(-2 * dbApq, dbAqq - dbApp);

		T dbSinTheta = sin(dbAngle);
		T dbCosTheta = cos(dbAngle);
		T dbSin2Theta = sin(2 * dbAngle);
		T dbCos2Theta = cos(2 * dbAngle);

		A[nRow][nRow] = dbApp * dbCosTheta * dbCosTheta +
			dbAqq * dbSinTheta * dbSinTheta + 2 * dbApq * dbCosTheta * dbSinTheta;
		A[nCol][nCol] = dbApp * dbSinTheta * dbSinTheta +
			dbAqq * dbCosTheta * dbCosTheta - 2 * dbApq * dbCosTheta * dbSinTheta;
		A[nRow][nCol] = 0.5 * (dbAqq - dbApp) * dbSin2Theta + dbApq * dbCos2Theta;
		A[nCol][nRow] = A[nRow][nCol];

		for (int i = 0; i < A.rows; i++)
		{
			if ((i != nCol) && (i != nRow))
			{
				int u = i * A.rows + nRow;
				int w = i * A.rows + nCol;
				dbMax = A.data[u];
				A.data[u] = A.data[w] * dbSinTheta + dbMax * dbCosTheta;
				A.data[w] = A.data[w] * dbCosTheta - dbMax * dbSinTheta;
			}
		}

		for (int j = 0; j < A.rows; j++)
		{
			if ((j != nCol) && (j != nRow))
			{
				int u = nRow * A.rows + j;
				int w = nCol * A.rows + j;
				dbMax = A.data[u];
				A.data[u] = A.data[w] * dbSinTheta + dbMax * dbCosTheta;
				A.data[w] = A.data[w] * dbCosTheta - dbMax * dbSinTheta;
			}
		}

		for (int i = 0; i < Q.rows; i++)
		{
			int u = i * Q.rows + nRow;
			int w = i * Q.rows + nCol;

			dbMax = Q.data[u];
			Q.data[u] = Q.data[w] * dbSinTheta + dbMax * dbCosTheta;
			Q.data[w] = Q.data[w] * dbCosTheta - dbMax * dbSinTheta;
		}

	}

	for (int i = 0; i < A.rows; i++) {
		T m = 0;
		int c = i;
		T value;
		for (int j = i; j < A.rows; j++) {
			if (A[j][j] > m) {
				m = A[j][j];
				c = j;
			}
		}
		if (c != i) {
			value = A[c][c];
			A[c][c] = A[i][i];
			A[i][i] = value;
			for (int j = 0; j < A.rows; j++) {
				value = Q[c][j];
				Q[c][j] = Q[i][j];
				Q[i][j] = value;
			}
		}
	}
}


template<typename T>
void EVD_Q(MAT<T>& A, MAT<T>& E, MAT<T>& Q, T eps, EVD_type type) {
	switch (type)
	{
	case _QR: {
		for (int i = 0; i < E.rows * E.cols; i++) {
			E.data[i] = A.data[i];
		}
		EVD_Q_QR(E, Q, eps);
		break;
	}
	case _Jacobi: {
		for (int i = 0; i < E.rows * E.cols; i++) {
			E.data[i] = A.data[i];
		}
		EVD_Q_Jacobi(E, Q, eps);
		break;
	}
	default:
		break;
	}
}