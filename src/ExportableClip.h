#pragma once

#include "NodeAnimation.h"
#include "ExportableItem.h"
#include "Arguments.h"
#include "ExportableScene.h"

class ExportableClip
{
public:
	ExportableClip(const Arguments& args, const AnimClipArg& clipArg, const ExportableScene& scene);
	virtual ~ExportableClip();

	GLTF::Animation glAnimation;

private:
	std::vector<float> m_timesPerFrame;
	std::unique_ptr<GLTF::Accessor> m_inputs;

	std::vector<std::unique_ptr<NodeAnimation>> m_nodeAnimations;

	DISALLOW_COPY_MOVE_ASSIGN(ExportableClip);
};
