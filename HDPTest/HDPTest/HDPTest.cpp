/*******************************************************************

HDPTest.cpp		developed by naka_t	2011.02.10

	HDP�N���X�̎g�p��

  Copyright (C) 2011  naka_t <naka_t@apple.ee.uec.ac.jp>
 *******************************************************************/
#include "stdafx.h"
#include "HierarchicalDP.h"
#include "utility.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CHierarchicalDP hdp;	// HDP�N���X
	int num, dim;			// �f�[�^�̐��Ǝ���

	// �f�[�^�ǂݍ���
	double **data = LoadMatrix<double>( dim , num , "sample.txt" );

	// �f�[�^���N���X�ɓn��
	hdp.SetData( data , num , dim );

	// gibbs sampling �Ńp�����^���X�V
	for(int i=0 ; i<100 ; i++ )
	{
		hdp.Update();
	}

	// ���ʂ�ۑ�
	hdp.SaveResult( "result" );

	Free( data );

	return 0;
}

