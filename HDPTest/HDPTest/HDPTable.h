#pragma once
/*******************************************************************

HDPTable.h		developed by naka_t	2011.01.12

	HDP�̃e�[�u���̏���ێ�����N���X

  Copyright (C) 2011  naka_t <naka_t@apple.ee.uec.ac.jp>
*******************************************************************/
#include <vector>


class CHDPTable
{
public:
	CHDPTable(void);
	~CHDPTable(void);

	void AddData(int w );		// �e�[�u���ɐl�i�f�[�^�j������
	void DeleteData( int w );	// �e�[�u������l�i�f�[�^�j������
	void PutDish( int k );		// �����i�g�s�b�N�j���e�[�u���ɒu��
	void DeleteDish( int k );	// �S�`�F�[���X���痿�����Ȃ��Ȃ����ꍇ�Ƀ��j���[��ύX

	int GetDataNum(){ return (int)m_datasOnTable.size(); }			// �e�[�u���ɍ����Ă���l�����擾
	int GetDishID(){ return m_dishID; }							// �e�[�u���ɒu����Ă��闿�����擾
	std::vector<int> &GetDatas(){ return m_datasOnTable; }		// �e�[�u���ɍ����Ă���l�i�f�[�^�j���擾

protected:
	std::vector<int> m_datasOnTable;		// �e�[�u���ɒu����Ă���f�[�^
	int m_dishID;							// ����ID
};
