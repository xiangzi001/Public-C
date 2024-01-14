
template <typename T>
void LU(MAT<T>& A, MAT<T>& L, MAT<T>& U)
{
	for (int i = 0; i < L.rows; i++)
	{
		//更新L矩阵  下三角矩阵
		for (int j = 0; j <= i; j++)
		{
			if (j == i) L[i][i] = 1;
			else
			{
				L[i][j] = A[i][j];
				for (int k = 0; k < j; k++)
				{
					L[i][j] -= L[i][k] * U[k][j];
				}
				L[i][j] /= U[j][j];
			}
		}

		//更新U矩阵  上三角矩阵
		for (int j = i; j < L.rows; j++)
		{
			U[i][j] = A[i][j];
			for (int k = 0; k < i; k++)
			{
				U[i][j] -= L[i][k] * U[k][j];
			}
		}
	}
}
