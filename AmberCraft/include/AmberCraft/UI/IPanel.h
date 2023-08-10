#pragma once

namespace AmberCraft::UI
{
	class IPanel
	{
	public:
		virtual ~IPanel() = default;

		virtual void Initialize() = 0;
		virtual void Draw() = 0;
	};
}
