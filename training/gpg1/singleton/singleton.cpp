/* Copyright (C) Scott Bilas, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */
#include <cassert>
#include <iostream>
#include <string>
#include <map>

// non multi-threading

template<typename T> class Singleton
{
	static T* ms_Singleton;

public:
	Singleton(void)
	{
		assert(!ms_Singleton);
		int offset = (int) (T*) 1 - (int) (Singleton<T>*) (T*) 1;
		ms_Singleton = (T*) ((int) this + offset);
	}
	~Singleton(void)
	{
		assert(ms_Singleton);
		ms_Singleton = 0;
	}
	static T& GetSingleton(void)
	{
		assert(ms_Singleton);
		return (*ms_Singleton);
	}
	static T* GetSingletonPtr(void)
	{
		return (ms_Singleton);
	}
};

template<typename T> T* Singleton<T>::ms_Singleton = 0;

// Sample usage:
//

using namespace std;

class Texture
{
	string mName;
public:
	Texture(const char* name) :
			mName(name)
	{
	}
	string GetName()
	{
		return mName;
	}
};

class TextureMgr: public Singleton<TextureMgr>
{
	map<string, Texture*> mTextures;
public:
	Texture* GetTexture(const char* name)
	{
		return mTextures[name];
	}
	void SetTexture(Texture* texture)
	{
		mTextures[texture->GetName()] = texture;
	}
	// ...
};

Texture stone1("stone1");

int main(void)
{
	TextureMgr::GetSingleton().SetTexture(&stone1);
	cout << TextureMgr::GetSingleton().GetTexture("stone1")->GetName() << endl;
	return 0;
}

