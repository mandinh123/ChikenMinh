#pragma once

#include "CommonFunc.h"

class BaseObject
{
public:
	BaseObject();
	~BaseObject();
	void SetRect(const int& x, const int& y, const int& w, const int& h)
	{
		rect_.x=x;
		rect_.y=y;
		rect_.w=w;
		rect_.h=h;
	}
	void SetPos(const int& x, const int& y)
	{
		rect_.x = x;
		rect_.y = y;
	}
	SDL_Rect GetRect() const {return rect_;}
	SDL_Texture* GetObject() const {return p_object_;}

	virtual bool LoadImg(std::string path, SDL_Renderer* screen);// dung loadImg nhung co chinh sua trong cac lop con
	void Render(SDL_Renderer* des,const SDL_Rect* clip = NULL);
	void Free();

//protected:
	SDL_Texture* p_object_;
	SDL_Rect rect_;

};
