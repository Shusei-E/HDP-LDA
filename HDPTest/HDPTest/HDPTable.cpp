#include "StdAfx.h"
#include "HDPTable.h"
#include <algorithm>

CHDPTable::CHDPTable(void) : m_dishID(-1)
{
}

CHDPTable::~CHDPTable(void)
{
}


void CHDPTable::AddData(int w )
{
	m_datasOnTable.push_back(w);
}


void CHDPTable::DeleteData( int w )
{
	std::vector<int>::iterator it = std::find( m_datasOnTable.begin() , m_datasOnTable.end() , w );
	if( it == m_datasOnTable.end() )
	{
		printf("Error : �e�[�u����%d�͑��݂��Ȃ�\n" , w );
	}
	else
	{
		m_datasOnTable.erase( it );
	}
}


void CHDPTable::PutDish( int k )
{
	m_dishID = k;
}

void CHDPTable::DeleteDish( int k )
{
	// �������Ȃ��Ȃ����̂Ń��j���[�ԍ����X�V
	if( m_dishID > k )
	{
		m_dishID--;
	}
}