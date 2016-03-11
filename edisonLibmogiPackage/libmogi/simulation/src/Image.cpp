/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *   Proprietary and confidential.                                            *
 *                                                                            *
 *   Unauthorized copying of this file via any medium is strictly prohibited  *
 *   without the explicit permission of Mogi, LLC.                            *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   http://www.binpress.com/license/view/l/0088eb4b29b2fcff36e42134b0949f93  *
 *                                                                            *
 *****************************************************************************/

#include "texture.h"

#include <math.h>

using namespace std;

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;

	Image8::Image8(int width, int height)
	: mWidth(width), mHeight(height) {
		mData = new Pixel[mWidth*mHeight];
	}

	Image8::~Image8() {
		delete [] mData;
	}

	Pixel& Image8::operator() (const int& x, const int& y) {
		return mData[x + y*mWidth];
	}

	const void* Image8::data() const {
		return mData;
	}

	const int& Image8::width() const {
		return mWidth;
	}

	const int& Image8::height() const {
		return mHeight;
	}

	void Image8::DrawPixel(unsigned int x, unsigned int y, const Pixel& color) {
		if (x < mWidth && y < mHeight) {
			(*this)(x, y) = color;
		}
	}

	void Image8::DrawLine(float x0, float y0, float x1, float y1, const Pixel& color) {
		float deltax = x1 - x0;
		float deltay = y1 - y0;
		float deltax1 = fabs(deltax);
		float deltay1 = fabs(deltay);
		float px = 2*deltay1-deltax1;
		float py = 2*deltax1-deltay1;
		int x, y, xe, ye, i;

		if(deltay1<=deltax1)
		{
			if(deltax>=0)
			{
				x=x0;
				y=y0;
				xe=x1;
			}
			else
			{
				x=x1;
				y=y1;
				xe=x0;
			}
			DrawPixel(x, y, color);
			for(i=0;x<xe;i++)
			{
				x=x+1;
				if(px<0)
				{
					px=px+2*deltay1;
				}
				else
				{
					if((deltax<0 && deltay<0) || (deltax>0 && deltay>0))
					{
						y=y+1;
					}
					else
					{
						y=y-1;
					}
					px=px+2*(deltay1-deltax1);
				}
				DrawPixel(x, y, color);
			}
		}
		else
		{
			if(deltay>=0)
			{
				x=x0;
				y=y0;
				ye=y1;
			}
			else
			{
				x=x1;
				y=y1;
				ye=y0;
			}
			DrawPixel(x, y, color);
			for(i=0;y<ye;i++)
			{
				y=y+1;
				if(py<=0)
				{
					py=py+2*deltax1;
				}
				else
				{
					if((deltax<0 && deltay<0) || (deltax>0 && deltay>0))
					{
						x=x+1;
					}
					else
					{
						x=x-1;
					}
					py=py+2*(deltax1-deltay1);
				}
				DrawPixel(x, y, color);
			}
		}
	}

	void Image8::DrawThickLine(int x0, int y0, int x1, int y1, float wd, const Pixel& color)
	{
		// Ok this is bad but I'm more interested in just getting it to work:
//		Image8 priorImage(width(), height());
//		memcpy((void*)priorImage.data(), data(), sizeof(Pixel)*width()*height());

		//http://members.chello.at/~easyfilter/bresenham.html
		float dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
		float dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
		float err = dx-dy, e2, x2, y2;                          /* error value e_xy */
		float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);
		float scale = 1.0;
		for (wd = (wd+1)/2; ; ) {                                   /* pixel loop */
//			setPixelColor(x0,y0,max(0,255*(abs(err-dx+dy)/ed-wd+1)));
			Pixel newColor;// = color;
			scale = min(max((float)0.0, fabsf(err - dx + dy)/ed-wd+1), (float)1.0);
			if (x0 < width() && y0 < height() && x0 >= 0 && y0 >= 0) {
				newColor.r = ((float)color.r)*(1.0-scale) + scale*((float)((*this)(x0,y0).r));
				newColor.g = ((float)color.g)*(1.0-scale) + scale*((float)((*this)(x0,y0).g));
				newColor.b = ((float)color.b)*(1.0-scale) + scale*((float)((*this)(x0,y0).b));
				DrawPixel(x0, y0, newColor);
			}
			e2 = err; x2 = x0;//-wd*sx;
			if (2*e2 >= -dx) {                                           /* x step */
				for (e2 += dy, y2 = y0-wd*sy*0; e2 < ed*wd && (y1 != y2+sy*wd || dx > dy); e2 += dx) {
					scale = min(max((float)0.0, fabsf(e2)/ed-wd+1), (float)1.0);
					y2 += sy;
					if (x0 < width() && y2 < height() && x0 >= 0 && y2 >= 0) {
						newColor.r = ((float)color.r)*(1.0-scale) + scale*((float)((*this)(x0,y2).r));
						newColor.g = ((float)color.g)*(1.0-scale) + scale*((float)((*this)(x0,y2).g));
						newColor.b = ((float)color.b)*(1.0-scale) + scale*((float)((*this)(x0,y2).b));
						DrawPixel(x0,  y2, newColor);
					}
					//					setPixelColor(x0, y2 += sy, max(0,255*(abs(e2)/ed-wd+1)));

				}
				if (x0 == x1) break;
				e2 = err; err -= dy; x0 += sx;
			}
			if (2*e2 <= dy) {                                            /* y step */
				for (e2 = dx-e2; e2 < ed*wd && (x1 != x2+sx*wd || dx < dy); e2 += dy) {
					scale = min(max((float)0.0, fabsf(e2)/ed-wd+1), (float)1.0);

					x2 += sx;
					if (x2 < width() && y0 < height() && x2 >= 0 && y0 >= 0) {
						newColor.r = ((float)color.r)*(1.0-scale) + scale*((float)((*this)(x2,y0).r));
						newColor.g = ((float)color.g)*(1.0-scale) + scale*((float)((*this)(x2,y0).g));
						newColor.b = ((float)color.b)*(1.0-scale) + scale*((float)((*this)(x2,y0).b));
						DrawPixel(x2,  y0, newColor);
					}
					//					setPixelColor(x2 += sx, y0, max(0,255*(abs(e2)/ed-wd+1)));

				}
				if (y0 == y1) break;
				err += dx; y0 += sy;
			}
		}
	}

	void Image8::DrawCircle(int x0, int y0, int radius, const Pixel& color)
	{
		int x = radius;
		int y = 0;
		int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

		while( y <= x )
		{
			DrawPixel(  x + x0,  y + y0, color ); // Octant 1
			DrawPixel(  y + x0,  x + y0, color ); // Octant 2
			DrawPixel( -x + x0,  y + y0, color ); // Octant 3
			DrawPixel( -y + x0,  x + y0, color ); // Octant 4
			DrawPixel( -x + x0, -y + y0, color ); // Octant 5
			DrawPixel( -y + x0, -x + y0, color ); // Octant 6
			DrawPixel(  x + x0, -y + y0, color ); // Octant 7
			DrawPixel(  y + x0, -x + y0, color ); // Octant 8

			y++;
			if (decisionOver2<=0)
			{
				decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
			} else {
				x--;
				decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
			}
		}
	}

	void Image8::DrawCircle(int x0, int y0, int radius, int thickness, const Pixel& color) {
		// I doubt this is efficient.
		for (int i = radius-thickness/2; i <= radius+thickness/2; i++) {
			DrawCircle(x0, y0, i, color);
		}
	}
	
#ifdef _cplusplus
}
#endif
