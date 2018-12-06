#include "stdafx.h"
#include "GameStateManager.h"

void GameStateManager::CleanAll()
{
	while (!m_states.empty())
	{
		m_states.back()->Cleanup();
		m_states.pop_back();
	}
}

GameStateManager::GameStateManager()
	: m_running{ true }, m_restart{ false }, state_cond{ NONE },
	  next_game_state{ nullptr }, m_transition_active{ false }
{ 
	CoreSystem::SetName("GSM");
}

void GameStateManager::Init()
{
	// @todo: Establish the normal game scene (Typically should be the splash screen)
#ifndef EDITOR
	PushState(std::make_unique<GameScene>("Resources/Savefile/Levels/MainMenu.lvl"));
#endif

}

void GameStateManager::Update()
{
	if (!m_states.empty())
	{
		// On runtime, we will check if the transition state is there
		if (m_transition_active)
			// Check if it has completed the transition
			if (m_states.back()->HasTransitionCompleted())
			{
				// Get the new xml file and progress into the new transition
				GameScene* curr_scene = dynamic_cast<GameScene*>(m_states.back().get());
				std::string& next_level = curr_scene->next_level;
				// @todo: To change this if necessary
				ChangeState(std::make_unique<GameScene>(next_level));
			}

		// If the scene has finished transitioning, we will still update regardless
		m_states.back()->Update(this);
	}
}

void GameStateManager::HandleEvents()
{
	if (!m_states.empty())
		m_states.back()->HandleEvents(this);
}

void GameStateManager::Draw()
{
	if (!m_states.empty())
		m_states.back()->Draw(this);
}

void GameStateManager::Cleanup()
{
	// Popping will require destroying all of the elements one by one and calling cleanup on every function
	while (!m_states.empty())
	{
		m_states.back()->Cleanup();
		m_states.pop_back();
	}
}

void GameStateManager::PushState(game_state && new_scene)
{
	// Capture the next scene for GSM to introduce a new element
	next_game_state = std::move(new_scene);
	// Change the variable for the next loop to adjust the changes
	state_cond = PUSH_STATE;
}

void GameStateManager::PopState()
{
	// Since we will do the necessary state changes at the start of the loop, we just have to set the variable
	state_cond = POP_STATE;
}

void GameStateManager::ChangeState(game_state && new_scene)
{
	// Capture the next scene for GSM to introduce a new element
	next_game_state = std::move(new_scene);
	// Change the variable for the next loop to adjust the changes
	state_cond = CHANGE_STATE;
}

GameState * GameStateManager::GetCurrentState()
{
	return m_states.empty() ? nullptr : m_states.back().get();
}

void GameStateManager::HandleStateChanges()
{
	bool state_init = false, state_resume = false;

	// Check if the restart condition is active
	if (m_restart && state_cond == NONE)
	{
		m_restart = false;
		// Call for restart in the game scene
		if (!m_states.empty())
		{
			GameScene* gs = static_cast<GameScene*>(m_states.back().get());
			gs->RestartScene();
		}
	}

	// Handle the new scenes (or old scenes) depending on the state_cond
	switch (state_cond)
	{
	case PUSH_STATE:
		// Idea is to pause the current top state and init the next state
		if (!m_states.empty())
			m_states.back()->Pause(false);
		m_states.emplace_back(std::move(next_game_state));
		state_init = true;
		break;

	case POP_STATE:
		// Clears the top of the stack and resume the next state
		if (!m_states.empty())
		{
			// Does not assume that there will always be a state

			if (m_to_quit)
			{
				// Clear up everything
				CleanAll();
				SetQuit(true);
			}
			else
			{
				// Otherwise we clean only the "top" layer
				m_states.back()->Cleanup();
				m_states.pop_back();

				state_resume = true;
			}
		}
		else
			SetQuit(true);
		break;

	case CHANGE_STATE:
		// Cleanup the current state and switch to the new state
		if (!m_states.empty())
		{
			// @todo: Might have to cater for events like pause menu
			m_states.back()->Cleanup();
			m_states.pop_back();
		}
		m_states.emplace_back(std::move(next_game_state));
		state_init = true;
		break;

	case PAUSED_STATE:
		// This will perform the codes for the actual pausing of the game
		if (!m_states.empty())
			m_states.back()->Pause(true);
		break;

	case RESUME_STATE:
		// This will perform the codes for the actual resuming of the game (from the pause menu)
		if (!m_states.empty())
			m_states.back()->Resume(true);
		break;

	case TRANSITION_IN_STATE:
		// When we transition, we would want to call for the function and wait for it to finish before we continue
		if (!m_states.empty())
		{
			m_states.back()->TransitionIn();
			// Notify that it is transitioning
			m_transition_active = true;
		}
		break;

	case TRANSITION_OUT_STATE:
		// When we transition, we would want to call for the function and wait for it to finish before we continue
		if (!m_states.empty())
		{
			m_states.back()->TransitionOut();
			// Notify that it is transitioning
			m_transition_active = true;
		}
		break;
	}

	// Handle boolean cases
	if (state_init)
	{
		auto& scene = m_states.back();
		scene->Init();
		// Change our scene data through the graphics
		//GFX_S.SetupSceneData(static_cast<GameScene*>(scene.get())->allGameSpaces);
	}
	else if (state_resume)
		m_states.back()->Resume(false);

	// Reset the state_cond
	state_cond = NONE;
}

void GameStateManager::RestartScene(bool active)
{
	m_restart = active;
}

void GameStateManager::SetSpecialState(GSM_STATE state)
{
	state_cond = state;
}

void GameStateManager::SetExitProgram(bool checker)
{
	m_to_quit = checker;
}

void GameStateManager::SelfUpdate()
{
	CoreSystem::PreUpdate();
	CoreSystem::UpdateWndFocus();
	if (CoreSystem::GetOverrideUpdate() || CoreSystem::IsActive())
	{
		HandleEvents();
		Update();
	}
	CoreSystem::PostUpdate();
}
