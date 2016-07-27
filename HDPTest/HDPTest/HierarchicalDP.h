#pragma once
/*******************************************************************

HierarchicalDP.h		developed by naka_t	2010.1.12

	�K�w�f�B���N���ߒ�

  Copyright (C) 2011  naka_t <naka_t@apple.ee.uec.ac.jp>
 *******************************************************************/
#include "HDPTable.h"
#include "HDPDish.h"

#define BUFFER_SIZE				10000	// �v�Z�p�̃o�b�t�@�T�C�Y�i�ŏ��Ɉꊇ�Ŋm�ہj
#define HDP_CONCPARA_PRIOR_A	1.0		// �K���}���O���z�̃p�����^	
#define HDP_CONCPARA_PRIOR_B	1.0		// �K���}���O���z�̃p�����^

double RandF();
unsigned int RandD(unsigned int max );

class CHierarchicalDP
{
public:
	CHierarchicalDP(void);
	~CHierarchicalDP(void);

	
	void SetData( double *data[] , int dataNum , int dataDim );		// �f�[�^�̃Z�b�g
	void Update();													// GibbsSampling�ɂ��e�[�u���Ɨ������T���v�����O
	void SaveResult( const char *dir );								// ���ʂ̕ۑ�
	std::vector<std::vector<double>> GetPz_dk();					// �e�f�[�^�̖ޓx���擾
	std::vector<int> GetClassificationResult_d();					// ���ތ��ʂ��擾

protected:
	/* 
	�ڔ���
	 d : �h�L�������g�̃C���f�b�N�X�i���̐��j
	 t : �e�[�u���̃C���f�b�N�X
	 k : �����̃C���f�b�N�X
	 w : �P��̃C���f�b�N�X
	 l : �����̒����i�P�ꐔ�j�̃C���f�b�N�X
	*/
	struct Document{						// �`�F�[���X�i�����j�\����
		int length;							// �l�̐��i�����̒����j
		int *wordID_l;						// �e�l�i�P��j��ID
		int *table_l;						// �e�l�i�P��j�������Ă���e�[�u��
		std::vector<CHDPTable> tables_t;	// �X�܂ɒu����Ă���e�[�u��
	}*m_documents_d;

	std::vector<CHDPDish> m_dishes_k;	// ����
	int m_dataDim;
	int m_dataNum;
	int *m_dataIndices_d;				// �f�[�^�̃C���f�b�N�X�i�f�[�^�̏��Ԃ����ւ��邽�߂ɕK�v�j
	double *m_P;						// �v�Z�p�o�b�t�@

	void Release();						// ���������
	
	void UpdateTable( Document &d , int l );						// �e�[�u���̍X�V
	int SamplingTable( std::vector<CHDPTable> &tables , int w );	// �e�[�u���̃T���v�����O
	void DeleteEmptyTables( Document &d );							// �N�������Ă��Ȃ��e�[�u�����폜
	void DeleteTable( Document &d , int t );						// �e�[�u��t���폜

	void UpdateDish( CHDPTable &table );							// �����̍X�V
	int SamplingDish( std::vector<int> &w );						// �������T���v�����O
	void DeleteEmptyDish();											// �N���H�ׂĂ��Ȃ��������폜
	void DeleteDish(int k);											// ����k���폜
	void PutDishOnTable( CHDPTable &table , int k );				// �e�[�u���ɗ���k��u��
	void RemoveDishFromTable( CHDPTable &table , int k );			// �e�[�u�����痿�����폜

	double m_lambda;							// �e�[�u���𐶐�����m���̌W����
	double m_gamma;								// �����𐶐�����m���̌W����
	double SamplingLambda( double oldLambda );	// �ɂ̃T���v�����O
	double SamplingGamma( double oldGamma );	// ���̃T���v�����O
};
