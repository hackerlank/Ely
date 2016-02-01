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
 * \file /Ely/training/particles/SpriteParticleRendererExt.cpp
 *
 * \date 2016-01-31
 * \author consultit
 */

#include "Utilities/Tools.h"
#include "SpriteParticleRendererExt.h"

namespace ely
{

std::string SpriteParticleRendererExt::sourceTextureName = "",
		SpriteParticleRendererExt::sourceFileName = "",
		SpriteParticleRendererExt::sourceNodeName = "";

SpriteParticleRendererExt::SpriteParticleRendererExt()
{
}

SpriteParticleRendererExt::~SpriteParticleRendererExt()
{
}

const std::string& SpriteParticleRendererExt::getSourceTextureName()
{
	if (mSourceTextureName == "")
	{
		SpriteParticleRendererExt::sourceTextureName = ConfigVariableString(
				'particle-sprite-texture', 'maps/lightbulb.rgb', "DConfig",
				ConfigFlags::F_dconfig).get_value();
	}
	// Return instance copy of class variable
	return mSourceTextureName;
}

void SpriteParticleRendererExt::setSourceTextureName(const std::string& name)
{
	// Set instance copy of class variable
	mSourceTextureName = name;
}

bool SpriteParticleRendererExt::setTextureFromFile(const Filename& fileName)
{
	if (fileName == "")
	{
		fileName = getSourceTextureName();
	}

	Texture* t = TexturePool::load_texture(fileName);
	if (t != NULL)
	{
		set_texture(t, t->get_y_size());
		setSourceTextureName(fileName);
		return true;
	}

	PRINT_ERR_DEBUG("Couldn't find rendererSpriteTexture file: " << fileName);
	return false;
}

bool SpriteParticleRendererExt::addTextureFromFile(const Filename& fileName)
{
}

const Filename& SpriteParticleRendererExt::getSourceFileName()
{
}

void SpriteParticleRendererExt::setSourceFileName(const Filename& fileName)
{
}

const std::string& SpriteParticleRendererExt::getSourceNodeName()
{
}

void SpriteParticleRendererExt::setSourceNodeName(const std::string& name)
{
}

bool SpriteParticleRendererExt::setTextureFromNode(const std::string& modelName,
		const std::string& nodeName, bool sizeFromTexels)
{
}

bool SpriteParticleRendererExt::addTextureFromNode(const std::string& modelName,
		const std::string& nodeName, bool sizeFromTexels)
{
}

} /* namespace ely */
