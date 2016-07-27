#include "StdAfx.h"
#include "HierarchicalDP.h"
#include "utility.h"
#include "randlib/randlib.h"
#include <string.h>
#include <float.h>
#include <math.h>
#include <windows.h>
#include <algorithm>



CHierarchicalDP::CHierarchicalDP(void)
{
	m_documents_d = NULL;
	m_dataDim = 0;
	m_dataNum = 0;
	m_dataIndices_d = NULL;
	m_P = new double[BUFFER_SIZE];

	// �K���}���z�̊��Ғl x�`Gamma(A,B) �� E(x) = A/B
	m_lambda = HDP_CONCPARA_PRIOR_A / HDP_CONCPARA_PRIOR_B;
	m_gamma = HDP_CONCPARA_PRIOR_A / HDP_CONCPARA_PRIOR_B;
}

CHierarchicalDP::~CHierarchicalDP(void)
{
	delete [] m_P;
}

void CHierarchicalDP::Release()
{
	for(int d=0 ; d<m_dataNum ; d++ )
	{
		delete [] m_documents_d[d].wordID_l;
		delete [] m_documents_d[d].table_l;
	}
	
	delete [] m_documents_d;
	m_documents_d = NULL;

	delete [] m_dataIndices_d;
	m_dataIndices_d = NULL;
}

void CHierarchicalDP::SetData( double *data[] , int dataNum , int dataDim )
{
	Release();

	m_dataNum = dataNum;
	m_dataDim = dataDim;


	// �������m��
	m_documents_d = new Document[dataNum];
	m_dataIndices_d = new int[dataNum];


	for(int d=0 ; d<m_dataNum ; d++ )
	{
		int length = 0;

		// �f�[�^�̃C���f�b�N�X���i�[
		m_dataIndices_d[d] = d;

		// ���v���v�Z
		for(int w=0 ; w<m_dataDim ; w++ ) length += (int)data[d][w];

		m_documents_d[d].length = length;
		m_documents_d[d].wordID_l = new int[length];
		m_documents_d[d].table_l = new int[length];
		m_documents_d[d].tables_t.push_back( CHDPTable() );	// ��̃e�[�u���ǉ�

		// �P�����
		for(int w=0, l=0 ; w<dataDim ; w++ )
		{
			for(int j=0 ; j<(int)data[d][w] ; j++ , l++ )
			{
				m_documents_d[d].wordID_l[l] = w;
				m_documents_d[d].table_l[l] = -1; 
			}
		}
	}

	// ��ڂ̗���������Ă���
	m_dishes_k.clear();
	m_dishes_k.push_back( CHDPDish(dataDim) );
}


void CHierarchicalDP::Update()
{
	// �f�[�^�̃C���f�b�N�X�������_���ŕ��ёւ�
	std::random_shuffle( m_dataIndices_d , m_dataIndices_d+m_dataNum , RandD );

	// �e�[�u������
	for(int i=0 ; i<m_dataNum ; i++ )		// �e�`�F�[���X��for���[�v
	{
		int d = m_dataIndices_d[i]; 
		int len = m_documents_d[d].length;	// �q�̐l��
		for(int l=0 ; l<len ; l++ )
		{
			UpdateTable( m_documents_d[d] , l );
		}
	}


	// ��������
	for(int i=0 ; i<m_dataNum ; i++ )		// �e�`�F�[���X��for���[�v
	{
		int d = m_dataIndices_d[i];
		int T = (int)m_documents_d[d].tables_t.size()-1;	// �Ō�̃e�[�u���͋�̃e�[�u���Ȃ̂ŗ����͒u���Ȃ�
		for(int t=0 ; t<T ; t++ )
		{
			UpdateDish( m_documents_d[d].tables_t[t] );
		}
	}

	// concentrate parameter�̃A�b�v�f�[�g
	m_lambda = SamplingLambda( m_lambda );
	m_gamma = SamplingGamma( m_gamma );
}



void CHierarchicalDP::UpdateTable( Document &d , int l )
{
	int t = d.table_l[l];
	int w = d.wordID_l[l];
	//int k = d.dish_l[l];

	if( t!=-1 )
	{
		d.tables_t[t].DeleteData( w );		// �f�[�^���e�[�u������폜
		int k = d.tables_t[t].GetDishID();	// �e�[�u���ɒu����Ă��闿�����擾
		m_dishes_k[k].DeleteData( w );		// �f�[�^�𗿗�����폜
	}
	t = SamplingTable( d.tables_t , w );	// �e�[�u���ԍ����T���v�����O

	// �T���v�����O���ꂽ�e�[�u���ɍ���
	d.table_l[l] = t;
	d.tables_t[t].AddData( w );	

	// �V�����e�[�u���ɍ������ꍇ��̃e�[�u����ǉ�
	if( t == d.tables_t.size()-1 )
	{
		UpdateDish( d.tables_t[t] );			// �V�����e�[�u���̗���������
		d.tables_t.push_back( CHDPTable() );	// �V�����e�[�u���̂��߂̋�̃e�[�u�������
	}
						
	int k = d.tables_t[t].GetDishID();				// ������H�ׂ�l�ɒǉ�
	m_dishes_k[k].AddData( w );


	// ��ɂȂ����e�[�u�����폜
	DeleteEmptyTables( d );
}

void CHierarchicalDP::DeleteEmptyTables( Document &d )
{
	int numTable = (int)d.tables_t.size() - 1;

	// ��̃e�[�u����T��
	for(int t=0 ; t<numTable ; t++ )
	{
		if( d.tables_t[t].GetDataNum() == 0 )
		{
			DeleteTable( d , t );
			break;
		}
	}
}

void CHierarchicalDP::DeleteTable( Document &d , int t )
{
	int T = (int)d.tables_t.size()-1;		// �Ō�̃e�[�u���́A����̃e�[�u���Ȃ̂�-1
	int k= d.tables_t[t].GetDishID();	// �e�[�u���ɏ���Ă�������
	d.tables_t.erase( d.tables_t.begin() + t );

	// �����̐l�C��������
	m_dishes_k[k].DownPopularity();

	// �������e�[�u����id���Â炷
	for(int l=0 ; l<d.length ; l++ )
	{
		if( d.table_l[l] > t )
		{
			d.table_l[l]--;
		}
	}
}


int CHierarchicalDP::SamplingTable( std::vector<CHDPTable> &tables , int w )
{
	int numTables = (int)tables.size();
	int numDishes = (int)m_dishes_k.size();
	double *P = m_P;
	int newTable = -1;
	double max = -DBL_MAX;


	// �Ō�̃e�[�u���͋�̃e�[�u��
	if( numTables == 1 ) return 0;

	// �e�e�[�u���ɑ�����ΐ��ޓx
	for(int t=0 ; t<numTables-1 ; t++ )
	{
		int k = tables[t].GetDishID();
		P[t] = m_dishes_k[k].CalcLogLikilihood( w );
	}
	P[numTables-1] = m_dishes_k[numDishes-1].CalcLogLikilihood( w );

	// �ő�l��T��
	for(int t=0 ; t<numTables ; t++ ) if( max < P[t] ) max = P[t];


	// �l���������Ȃ肷���邽�߁A�ő�l�ň���
	// �e�e�[�u���̐l�C��������
	// �T���v�����O�̂��߂ɗݐϊm���ɂ���
	P[0] = exp(P[0] - max) * tables[0].GetDataNum();
	for(int t=1 ; t<numTables-1 ; t++ ) P[t] = P[t-1] + exp(P[t] - max) * tables[t].GetDataNum(); 

	// �V���ȃe�[�u���𐶐�����m��
	P[numTables-1] = P[numTables-2] + exp(P[numTables-1] - max) * m_lambda;

	// �T���v�����O���邽�߂̗����𔭐�
	double rand = RandF() * P[numTables-1];

	// �v�Z�����m���ɏ]���ĐV���ȃe�[�u����I��
	for(newTable=0 ; newTable<numTables-1 ; newTable++ )
	{
		if( P[newTable] >= rand ) break;
	}
	return newTable;
}


void CHierarchicalDP::UpdateDish( CHDPTable &table )
{
	int k = table.GetDishID();
	std::vector<int> &w = table.GetDatas();

	if( k!=-1 )
	{
		RemoveDishFromTable( table , k );	// �e�[�u�����痿����������
	}

	k = SamplingDish( w );					// �e�[�u���ɒ����Ă���l�ɐV���ȗ������T���v�����O

	// �V�����������I�����ꂽ�ꍇ�́A
	// vector�̍Ō�ɋ�̗����i�V���j���[���j��ǉ�
	if( k == m_dishes_k.size()-1 )
	{
		m_dishes_k.push_back( CHDPDish(m_dataDim) );
	}

	PutDishOnTable( table , k );	// �e�[�u���ɗ�����u��
	DeleteEmptyDish();				// �N���H�ׂĂȂ��������폜
}


int CHierarchicalDP::SamplingDish( std::vector<int> &w )
{
	int numDishes = (int)m_dishes_k.size();
	double *P = m_P;
	int newDish = -1;
	double max = -DBL_MAX;

	// �܂������͒N���H�ׂĂ��Ȃ��̂ŁA�V���j���[
	// index�̍Ōオ�V���j���[��\���Ă���
	if( numDishes == 1 ) return 0;

	// �lw������k���D�ޑΐ��ޓx
	for(int k=0 ; k<numDishes ; k++ )
	{
		P[k] = m_dishes_k[k].CalcLogLikilihood( w );
	}

	// �ő�l��T��
	for(int k=0 ; k<numDishes ; k++ ) if( max < P[k] ) max = P[k];


	P[0] = exp(P[0] - max) * m_dishes_k[0].GetPopularity();
	for(int k=1 ; k<numDishes-1 ; k++ ) P[k] = P[k-1] + exp(P[k] - max) * m_dishes_k[k].GetPopularity(); 

	// �V���ȃe�[�u���𐶐�����m��
	P[numDishes-1] = P[numDishes-2] + exp(P[numDishes-1] - max) * m_gamma;

	// �T���v�����O���邽�߂̗����𔭐�
	double rand = RandF() * P[numDishes-1];

	// �v�Z�����m���ɏ]���ĐV���ȃe�[�u����I��
	for(newDish=0 ; newDish<numDishes-1 ; newDish++ )
	{
		if( P[newDish] >= rand ) break;
	}

	return newDish;
}


void CHierarchicalDP::DeleteEmptyDish()
{
	int K = (int)m_dishes_k.size()-1;

	for(int k=0 ; k<K ; k++ )
	{
		if( m_dishes_k[k].GetPopularity() == 0 )
		{
			DeleteDish( k );
			break;
		}
	}
}

void CHierarchicalDP::DeleteDish(int k)
{
	m_dishes_k.erase( m_dishes_k.begin() + k );

	// �S�f�[�^�ƃe�[�u���̗���ID���X�V
	for(int d=0 ; d<m_dataNum ; d++ )
	{
		int T = (int)m_documents_d[d].tables_t.size();
		CHDPTable *tables = &(m_documents_d[d].tables_t[0]);

		for(int t=0 ; t<T ; t++ )
		{
			tables[t].DeleteDish( k );	// ���j���[�ύX
		}
	}
}


void CHierarchicalDP::PutDishOnTable( CHDPTable &table , int k )
{
	CHDPDish &dish = m_dishes_k[k];
	table.PutDish( k );						// ������u��
	dish.AddData( table.GetDatas() );		// �H�ׂĂ���l��ǉ�
	dish.UpPopularity();					// �����̐l�CUP
}

void CHierarchicalDP::RemoveDishFromTable( CHDPTable &table , int k )
{
	CHDPDish &dish = m_dishes_k[k];
	dish.DeleteData( table.GetDatas() );	// �H�ׂĂ���l�����O
	dish.DownPopularity();					// �����̐l�CDOWN
}



std::vector<std::vector<double>> CHierarchicalDP::GetPz_dk()
{
	std::vector<std::vector<double>> lik_dk;
	lik_dk.resize( m_dataNum );
	for(int d=0 ; d<m_dataNum ; d++ ) lik_dk[d].resize( m_dishes_k.size() );

	for(int d=0 ; d<m_dataNum ; d++ )
	{
		double sum = 0;
		std::vector<CHDPTable> &tables = m_documents_d[d].tables_t;
		for(int t=0 ; t<tables.size()-1 ; t++ )
		{
			int k = tables[t].GetDishID();
			double lik = tables[t].GetDataNum();
			lik_dk[d][k] += lik;
			sum += lik;
		}

		// ���K��
		for(int k=0 ; k<lik_dk[d].size() ; k++ )
		{
			lik_dk[d][k] /= sum;
		}
	}

	return lik_dk;
}

std::vector<int> CHierarchicalDP::GetClassificationResult_d()
{
	std::vector<std::vector<double>> Pz_dk = GetPz_dk();
	std::vector<int> classRes;

	for(int d=0 ; d<Pz_dk.size() ; d++ )
	{
		double max = -DBL_MAX;
		int maxIdx = -1;
		for(int k=0 ; k<Pz_dk[d].size() ; k++ )
		{
			if( max < Pz_dk[d][k] )
			{
				max = Pz_dk[d][k];
				maxIdx = k;
			}
		}
		classRes.push_back( maxIdx );
	}

	return classRes;
}



void CHierarchicalDP::SaveResult( const char *dir )
{
	char dirname[256];
	char filename[256];

	strcpy( dirname , dir );
	int len = (int)strlen( dir );
	if( len==0 || dir[len-1] != '\\' || dir[len-1] != '/' ) strcat( dirname , "\\" );
	
	CreateDirectory( dirname , NULL );

	std::vector< std::vector<double> > Pz = GetPz_dk();
	sprintf( filename , "%sPz.txt" , dirname );
	SaveMatrix( Pz , (int)Pz[0].size() , (int)Pz.size() , filename );

	std::vector<int> classRes = GetClassificationResult_d();
	sprintf( filename , "%sClusteringResult.txt" , dirname );
	SaveArray( classRes , (int)classRes.size() , filename );	

}


double CHierarchicalDP::SamplingLambda( double oldLambda )
{
	for(int i=0 ; i<50 ; i++ )
	{
		float gammaB = 0;	// �K���}�֐��X�P�[���p�����[�^
		float gammaA = 0;	// �K���}�֐��`��p�����[�^
		int numAllTables = 0;

		for(int d=0 ; d<m_dataNum ; d++ )
		{
			int len = m_documents_d[d].length;

			// �x�[�^���z����T���v������
			float w = genbet( (float)oldLambda+1 , (float)len );
			gammaB -= log(w);
			
			// ��l���z����T���v�����O
			int s = (RandF() * (oldLambda + len)) < len ? 1 : 0;
			gammaA -= s;

			// �e�[�u���̑������v�Z
			numAllTables += (int)m_documents_d[d].tables_t.size()-1;
		}

		// ���㕪�z�̃p�����^���v�Z
		gammaA += (float)(HDP_CONCPARA_PRIOR_A + numAllTables);
		gammaB += (float)HDP_CONCPARA_PRIOR_B;

		// �X�V
		oldLambda = (double)gengam( gammaB , gammaA );
	}

	return oldLambda;
}


double CHierarchicalDP::SamplingGamma( double oldGamma )
{
	// �e�[�u���̑������v�Z
	int numAllTables = 0;
	for(int k=0 ; k<m_dishes_k.size() ; k++ )
	{
		numAllTables += m_dishes_k[k].GetPopularity();
	}

	for(int i=0 ; i<20 ; i++ )
	{
		float gammaB = 0;	// �K���}�֐��X�P�[���p�����[�^
		float gammaA = 0;	// �K���}�֐��`��p�����[�^
		int numDish = (int)m_dishes_k.size();

		// �x�[�^���z����T���v������
		float w = genbet( (float)oldGamma+1 , (float)numAllTables );

		// ��l�̕��z���T���v�����O
		int s = (RandF() * (oldGamma + numDish)) < numDish ? 1 : 0;
		gammaA = (float)(HDP_CONCPARA_PRIOR_A + numDish - s);
		gammaB = (float)(HDP_CONCPARA_PRIOR_B - log(w));

		// �X�V
		oldGamma = (double)gengam( gammaB , gammaA );
	}

	return oldGamma;
}


double RandF()
{
	unsigned int val;
	rand_s(&val);
	return (double)val/UINT_MAX;
}

unsigned int RandD(unsigned int max )
{
	unsigned int val;
	rand_s(&val);
	return val%max;
}