#pragma once

#include "../Gateway/SimObject.h"
#include "../Gateway/Vec2.h"
#include "../Gateway/TextDocument.h"
#include "../Gateway/WindowEventReceiver.h"

#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/ElementDocument.h>

namespace Gateway
{

class UICanvas : public SimObject, public WindowEventFilter
{
public:
	static Ref<UICanvas> Create(Ref<RenderWindow> window, World* world, const std::string& name = std::string(), unsigned long long id = 0, SimObject* parent = nullptr);

public:
	void SetRmlDocument(Ref<TextDocument> document);

	Rml::ElementDocument* GetRmlRoot();

	bool WantsToConsumeInput();

protected:
	UICanvas(Ref<RenderWindow> window, World* world, const std::string& name, unsigned long long id, SimObject* parent);
	
	~UICanvas();

private:
	virtual void OnUpdate() override;

	virtual void OnExtractFrameRenderData(RenderData& renderData) override;
	
private:
	Rml::Context*			m_rmlContext  = nullptr;

	Rml::ElementDocument*	m_rmlDocument = nullptr;

	bool m_wantsToConsume = false;

	bool		 OnFilter(SDL_Event* event) override;
};

}