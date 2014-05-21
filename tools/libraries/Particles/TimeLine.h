/*
 * Copyright (c) 2005, Stephan Reiter <stephan.reiter@students.jku.at>,
 * Christian Wressnegger <christian.wressnegger@students.jku.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * The names of its contributors may not be used to endorse or promote
 * products derived from this software without specific prior written
 * permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __TIMELINE_H__
#define __TIMELINE_H__

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGBaseTypes.h>

template<class T>
class CTimeline
{
public:
  CTimeline();
  inline ~CTimeline() { Clear(); }

  void Clear();
  inline void SetDefaultValue( T i_defaultValue ) { m_defaultValue = i_defaultValue; }
  inline OSG::UInt32 iGetSize() const { return m_iSize; }
  inline OSG::Real32 rGetLength() const { return m_rLength; }

  void SetValue( OSG::Real32 i_rTime, T i_value );
  T GetValue( OSG::Real32 i_rTime ) const;
  T GetMaximum() const;

private:
  struct node
  {
    inline node() : pNext( 0 ) {}
    inline node( OSG::Real32 i_rTime, T i_value, node *i_pNext = 0 ) :
      rTime( i_rTime ), value( i_value ), pNext( i_pNext ) {}

    OSG::Real32 rTime;
    T       value;
    node    *pNext;
  } m_dummy;

  OSG::UInt32  m_iSize;
  OSG::Real32  m_rLength;
  T       m_defaultValue;
};

template<>
inline CTimeline<OSG::Real32>::CTimeline() : m_iSize( 0 ), m_rLength( 0.0f ), m_defaultValue( 0.0f ) {}

template<>
inline CTimeline<OSG::Vec3f>::CTimeline() : m_iSize( 0 ), m_rLength( 0.0f ), m_defaultValue( 0.0f, 0.0f, 0.0f ) {}

template<class T>
inline CTimeline<T>::CTimeline() : m_iSize( 0 ), m_rLength( 0.0f ) {}

template<class T>
void CTimeline<T>::Clear()
{
  node *pCur = m_dummy.pNext;
  while( pCur )
  {
    node *pNext = pCur->pNext;
    delete pCur;
    pCur = pNext;
  }
  m_dummy.pNext = 0;
  m_iSize = 0;
}

template<class T>
void CTimeline<T>::SetValue( OSG::Real32 i_rTime, T i_value )
{
  node *pLast = &m_dummy;
  while( pLast->pNext )
  {
    if( i_rTime == pLast->pNext->rTime )
    {
      pLast->pNext->value = i_value;
      return;
    }
    else if( i_rTime < pLast->pNext->rTime )
      break;

    pLast = pLast->pNext;
  }

  node *pNewNode = new node( i_rTime, i_value, pLast->pNext );
  if( pNewNode != 0 )
  {
    pLast->pNext = pNewNode;
    ++m_iSize;
    if( i_rTime > m_rLength ) m_rLength = i_rTime;
  }
}

template<class T>
T CTimeline<T>::GetValue( OSG::Real32 i_rTime ) const
{
  if( m_iSize == 0 )
    return m_defaultValue;
  else if( m_iSize == 1 )
    return m_dummy.pNext->value;
  else
  {
    node *pFirst = m_dummy.pNext;
    if( i_rTime <= pFirst->rTime )
      return pFirst->value;
    else
    {
      node *pCur = pFirst->pNext, *pLast = pFirst;
      while( pCur )
      {
        if( i_rTime <= pCur->rTime )
        {
          const OSG::Real32 fInterpolation = (i_rTime - pLast->rTime) / (pCur->rTime - pLast->rTime);
          return pLast->value * (1.0f - fInterpolation) + pCur->value * fInterpolation;
        }
        pLast = pCur; pCur = pCur->pNext;
      }
      return pLast->value;
    }
  }
}

template<class T>
T CTimeline<T>::GetMaximum() const
{
  if( m_iSize == 0 )
    return T();
  else
  {
    T maximum = m_dummy.pNext->value;

    node *pCur = m_dummy.pNext->pNext;
    while( pCur )
    {
      if( pCur->value > maximum )
        maximum = pCur->value;
      pCur = pCur->pNext;
    }

    return maximum;
  }
}

#endif // __TIMELINE_H__
