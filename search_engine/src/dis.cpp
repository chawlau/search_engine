#include"dis.h"
namespace DISTANCE
{
		int Edit::operator()(const std::string& src, const std::string& dest)
		{

			int row = length(dest)  ;
			int col = length(src)  ;
			int** pp = new int*[row + 1]() ;
			int index, res, i, j , i_dest, j_src;
			int val_i,val_d ,val_r;
			unsigned int  src_code, dest_code ;
			for(index = 0; index <= row; index ++)
			{
				pp[index] = new int[col + 1]() ;
			}

			for(index = 0; index <= row; index ++)
			{
				pp[index][0] = index ;
			}

			for(index = 0; index <= col; index ++)
			{
				pp[0][index] = index ;
			}
			for(i = 1 , i_dest = 0 ; i <= row; i ++)
			{
				if(dest[i_dest] > 0)
				{
					dest_code = dest[i_dest];
					i_dest ++ ;
				}else if( (dest[i_dest] & 0xE0 )== 0xC0)// 110
				{
					dest_code = (dest[i_dest]&0xff) << 8 | (dest[i_dest + 1]&0xff );

					//	dest_code = *(reinterpret_cast<unsigned short*>&dest[i_dest]);
					i_dest = i_dest + 2 ; ;

				}else if( (dest[i_dest] & 0xF0 ) ==0xE0 )//11100000
				{
					dest_code = (unsigned int)(dest[i_dest]&0xff) << 16 | (unsigned int)(dest[i_dest + 1]&0xff) << 8 | (unsigned int)(dest[i_dest + 2]&0xff);
						i_dest = i_dest + 3;
				}else 
				{
					dest_code =  (dest[i_dest]&0xff )<< 24 | (dest[i_dest + 1]&0xff) << 16 | (dest[i_dest + 2]&0xff )<<8 | (dest[i_dest + 3]&0xff) ;
					//	dest_code = *(reinterpret_cast<int*>&dest[i_dest]);

					i_dest = i_dest + 4;
				}


				for(j = 1, j_src = 0; j <= col; j ++)
				{
					if(src[j_src] > 0)
					{
						src_code = src[j_src];
						j_src ++ ;
					}else if( (src[j_src] & 0xE0 )== 0xC0)// 110
					{
						src_code = (src[j_src]&0xff) << 8 | (src[j_src + 1]&0xff );

						//	src_code = *(reinterpret_cast<unsigned short*>&src[i_src]);
						j_src = j_src + 2 ; ;

					}else if( (src[j_src] & 0xF0) ==0xE0 )//11100000
					{
						src_code = (src[j_src]&0xff )<< 16 | (src[j_src + 1]&0xff) << 8 | (src[j_src + 2]&0xff);
							j_src = j_src + 3;
					}else 
					{
						src_code = (src[j_src]&0xff )<< 24 | (src[j_src + 1]&0xff )<< 16 | (src[j_src + 2]&0xff)<<8 | (src[j_src + 3]&0xff) ;
						//	src_code = *(reinterpret_cast<int*>&src[i_src]);

						j_src = j_src + 4;
					}

					val_i = pp[i - 1][j] + 1 ;
					val_d = pp[i][j - 1] + 1;
					val_r = pp[i - 1][j - 1] + (src_code == dest_code ? 0: 1 );
					pp[i][j] = triple_min(val_i, val_d, val_r);
					//	std::cout << pp[i][j] << " " ;
				}
				//	std::cout << std::endl ;
			}

			res = pp[row][col];
			for(index = 0; index <= row; index ++)
			{
				delete [] pp[index] ;
			}

			delete [] pp ;

			return res ;
		}
		int Edit::triple_min(int a, int b, int c)//2 4 5
		{
			return a < b ?(a < c? a : c):(b < c ? b : c);
		}
		int Edit::length(const std::string& str)
		{
			int len = 0 ;
			int index ;
			for(index = 0; index < str.length(); )
			{
				if(str[index] > 0)
				{
					index ++ ;
					len ++ ;
				}else if( (str[index] & 0xE0 ) == 0xC0)// 110
				{
					index = index + 2 ;
					len ++ ;
				}else if( (str[index] & 0xF0 ) ==0xE0 )//11100000
				{
					index = index + 3 ;
					len ++ ;
				}else 
				{
					index = index + 4 ;
					len ++ ;
				}
			}
			return len ;
		}
}
