#pragma once
/*******************************************************************

HDPDish.h		developed by naka_t	2011.01.12

	HDP�̃e�[�u���ɒu����闿���̏���ێ�����N���X

  Copyright (C) 2011  naka_t <naka_t@apple.ee.uec.ac.jp>
*******************************************************************/
#include <vector>

#define HDP_ALPHA	0.1

class CHDPDish
{
public:
	CHDPDish(int wordNum );
	~CHDPDish(void);

	void AddData( int w );								// ���̗�����H�ׂĂ���l��ǉ�
	void AddData( std::vector<int> &w );				// ���̗�����H�ׂĂ��镡���̐l��ǉ�
	void DeleteData( int w );							// ���̗�����H�ׂĂ���l�����O
	void DeleteData( std::vector<int> &w );				// ���̗�����H�ׂĂ��镡���̐l�����O

	void UpPopularity();								// ���̗����̐l�C���グ��i�H�ׂ��Ă���e�[�u������ǉ��j
	void DownPopularity();								// ���̗����̐l�C��������i�H�ׂ��Ă���e�[�u�����������j
	int GetPopularity();								// ���̗����̐l�C���擾�i�H�ׂ�Ă���e�[�u�������擾�j

	double CalcLogLikilihood( int w );					// �l(w�F�P��)�����̗����i�g�s�b�N�j���D�ޖޓx	
	double CalcLogLikilihood( std::vector<int> w );		// �����̐l(w�F�P��)�����̗����i�g�s�b�N�j���D�ޖޓx	

protected:
	std::vector<int> m_N_w;		// ���̗�����H�ׂĂ���l�i�P��j�̐��̓���
	int m_N;					// ���̗�����H�ׂĂ���l�i�P��j�̍��v
	int m_numTables;			// ���̗������H�ׂ��Ă���e�[�u���̐�
	int m_wordNum;
};
