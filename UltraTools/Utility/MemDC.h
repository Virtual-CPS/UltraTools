#ifndef _MY_MEMDC_H_
#define _MY_MEMDC_H_

//////////////////////////////////////////////////
// CMyMemDC - memory DC
//
class CMyMemDC : public CDC
{
protected:
   CBitmap  m_bitmap;       
   CBitmap* m_oldBitmap;    
   CDC*     m_pDC;          
   CRect    m_rect;         
   BOOL     m_bMemDC;       
    
   void Build( CDC* pDC )
   {
        ASSERT(pDC != NULL); 

        m_pDC		= pDC;
        m_oldBitmap = NULL;
        m_bMemDC	= !pDC->IsPrinting();

        if( m_bMemDC )
		{
            CreateCompatibleDC(pDC);
            pDC->LPtoDP(&m_rect);

            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_oldBitmap = SelectObject(&m_bitmap);
            
            SetMapMode(pDC->GetMapMode());
            pDC->DPtoLP(&m_rect);
            SetWindowOrg(m_rect.left, m_rect.top);
        }
		else
		{
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }

        FillSolidRect( m_rect, pDC->GetBkColor() );
    }

public:
   CMyMemDC( CDC *pDC )
	   : CDC()
   {
	   pDC->GetClipBox( &m_rect );
	   Build( pDC );
   }

   CMyMemDC( HDC hDC )
	   : CDC()
   {
	   CDC::FromHandle( hDC )->GetClipBox( &m_rect );
	   Build( CDC::FromHandle( hDC ) );
   }

   CMyMemDC( CDC *pDC, const RECT& rect ) 
	   : CDC()
   {
	   m_rect = rect;
	   Build( pDC ); 
   }
    
   virtual ~CMyMemDC()
   {        
        if( m_bMemDC )
		{
            m_pDC->BitBlt( m_rect.left,
				           m_rect.top,
						   m_rect.Width(),
						   m_rect.Height(),
						   this,
						   m_rect.left,
						   m_rect.top,
						   SRCCOPY );            
            
            SelectObject(m_oldBitmap);        
        }
		else
		{
            m_hDC = m_hAttribDC = NULL;
        }    
    }
    
    CMyMemDC* operator->() 
    {
        return this;
    }    

    operator CMyMemDC*() 
    {
        return this;
    }
};
#endif