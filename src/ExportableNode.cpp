#include "externals.h"
#include "ExportableNode.h"
#include "MayaException.h"
#include "ExportableResources.h"
#include "NodeAnimation.h"
#include "Transform.h"
#include "ExportableScene.h"
#include "Arguments.h"

ExportableNode::ExportableNode(
	ExportableScene& scene,
	std::unique_ptr<ExportableNode>& owner,
	MDagPath dagPath)
	: ExportableObject(dagPath.node())
	, dagPath(dagPath)
	, scaleFactor(1)
{
	MStatus status;

	owner.reset(this);

	auto& resources = scene.resources();
	auto& args = resources.arguments();

	// Remember scale factor
	scaleFactor = args.scaleFactor;

	// Get name
	const auto name = dagPath.partialPathName(&status);
	THROW_ON_FAILURE(status);
	args.assignName(glNode, name.asChar());

	// Get parent
	const auto parentNode = scene.getParent(this);
	if (parentNode)
	{
		parentDagPath = parentNode->dagPath;
	}
	else
	{
		// Add root nodes to the scene
		scene.glScene.nodes.emplace_back(&glNode);
	}

	// Get transform
	const auto objectMatrix = Transform::getObjectSpaceMatrix(dagPath, parentDagPath);
	initialTransform = Transform::toTRS(objectMatrix, scaleFactor, name.asChar());
	THROW_ON_FAILURE(status);
	glNode.transform = &initialTransform;

	// Register as child
	if (parentNode)
	{
		parentNode->glNode.children.push_back(&glNode);
	}

	// Get mesh, but only if the node was selected.
	if (args.selection.hasItem(dagPath))
	{
		dagPath.extendToShape();

		if (dagPath.hasFn(MFn::kMesh)) 
		{
			m_mesh = std::make_unique<ExportableMesh>(scene, dagPath);
			glNode.mesh = &m_mesh->glMesh;

			// Link skin if mesh has skeleton
			if (m_mesh->glSkin.skeleton)
			{
				glNode.skin = &m_mesh->glSkin;
			}
		}
	}
}

ExportableNode::~ExportableNode() = default;

std::unique_ptr<NodeAnimation> ExportableNode::createAnimation(const int frameCount, const double scaleFactor)
{
	return std::make_unique<NodeAnimation>(*this, frameCount, scaleFactor);
}
