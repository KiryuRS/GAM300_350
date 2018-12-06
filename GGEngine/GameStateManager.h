#pragma once

class GameStateManager;
// Different states for GSM to cater at the start of every frame loop
enum GSM_STATE
{
	NONE,							// Monostate
	PUSH_STATE,						// New scene to the "top" of the layer
	CHANGE_STATE,					// Changing the current scene from the "top" of the layer
	POP_STATE,						// Removing the current scene from the "top" of the layer
	PAUSED_STATE,					// Pausing the current scene
	RESUME_STATE,					// Resuming the current scene
	TRANSITION_IN_STATE,			// Transition from a "black screen" to the next screen
	TRANSITION_OUT_STATE			// Transition from the current screen to a "black screen"
};


// Any Scenes will inherit this class
class GameState
{
	friend class GameStateManager;

protected:
	GameState()
		: m_transitioning{ false }, m_transition_active{ false }
	{ }

	// Transition in
	virtual void TransitionIn() = 0;
	// Transition out
	virtual void TransitionOut() = 0;

public:

	// For scene transition purpose
	bool m_transitioning, m_transition_active;

	// Execution at the beginning of each phase
	virtual void Init() = 0;
	// Destruction after a state has left (popped off the stack)
	virtual void Cleanup() = 0;
	// Push state will cause this function to activate
	virtual bool Pause(bool actual_pause) = 0;
	// Pop state will cause this function to activate (when applicable)
	virtual bool Resume(bool actual_pause) = 0;
	// Event handler
	virtual void HandleEvents(GameStateManager*) = 0;
	// Updating the state of the game
	virtual bool Update(GameStateManager*) = 0;
	// Draw the graphics
	virtual void Draw(GameStateManager*) = 0;

	virtual ~GameState() = default;
	virtual bool HasTransitionCompleted() const = 0;
};

typedef class GameStateManager final : public CoreSystem
{
	using game_state = std::unique_ptr<GameState>;

	bool m_running;								// boolean check to determine if the engine should continue to run
	bool m_restart;								// boolean check to determine if the current "scene" should be restarted
	bool m_transition_active;					// boolean check to determine if a scene is curently transitioning
	bool m_to_quit;								// boolean check to determine if the game wants to be exited
	GSM_STATE state_cond;						// Current state condition to cater for GSM behaviour
	game_state next_game_state;					// Next game state to include into our container
	std::vector<game_state> m_states;			// Container for the bunch of game states (using the last element as the latest element)

	void CleanAll();

	// Friends
	friend class CoreEngine;

public:

	GameStateManager();
	void Init();								// Initialize our GSM
	void Update();								// Updates the game scene(s)
	void HandleEvents();						// Handle any incoming events before the update
	void Draw();								// Calls the game scene to draw (graphics)
	void Cleanup();								// Clean up the GSM (ensure no mem leak)

	void PushState(game_state&& new_scene);		// Pause the current state and add a new state to the end of the stack
	void PopState();							// Removes the 
	void ChangeState(game_state&& new_scene);	// Override the current state with a new scene
	GameState* GetCurrentState();				// Gets the current state in the game
	void HandleStateChanges();					// Determine if there's any Push, Pop or Change of states involved
	void RestartScene(bool active);				// Restarts the entire scene

	inline bool IsRunning() const
	{
		return m_running;
	}
	inline void SetQuit(bool quit)
	{
		m_running = !quit;
	}
	inline bool GetRestartState() const
	{
		return m_restart;
	}

	// Inserting a different state for special cases
	void SetSpecialState(GSM_STATE state);
	void SetExitProgram(bool checker);
	void SelfUpdate() override;

} GSM;