/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/training/particles/SpriteParticleRendererExt.h
 *
 * \date 2016-01-31
 * \author consultit
 */
#ifndef SPRITEPARTICLERENDEREREXT_H_
#define SPRITEPARTICLERENDEREREXT_H_

namespace ely
{

#include <spriteParticleRenderer.h>

/**
 * \brief Contains methods to extend functionality of the SpriteParticleRenderer class.
 */
class SpriteParticleRendererExt: public SpriteParticleRenderer
{
private:
	// Initialize class variables for texture, source file and node for texture and
	// node path textures to None.  These will be initialized to a hardcoded default
	// or whatever the user specifies in his/her Configrc variable the first time they
	// are accessed
	// Will use instance copy of this in functions below
	static std::string sourceTextureName, sourceFileName, sourceNodeName;

public:
	SpriteParticleRendererExt();
	virtual ~SpriteParticleRendererExt();

	const std::string& getSourceTextureName();
	void setSourceTextureName(const std::string& name);
	bool setTextureFromFile(const Filename& fileName = "");
	bool addTextureFromFile(const Filename& fileName = "");
	const Filename& getSourceFileName();
	void setSourceFileName(const Filename& name);
	const std::string& getSourceNodeName();
	void setSourceNodeName(const std::string& name);
	bool setTextureFromNode(const std::string& modelName = "",
			const std::string& nodeName = "", bool sizeFromTexels = false);
	bool addTextureFromNode(const std::string& modelName = "",
			const std::string& nodeName = "", bool sizeFromTexels = false);

protected:
	std::string mSourceTextureName, mSourceFileName, mSourceNodeName;
};

///inline definitions

} /* namespace ely */

#endif /* SPRITEPARTICLERENDEREREXT_H_ */
