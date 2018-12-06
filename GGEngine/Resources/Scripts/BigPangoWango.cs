using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BigPangoWango : MonoBehaviour {

    enum State { Charge, Roll, SlowDown,Rest, BreakUp}

    State state = State.Charge;

    float curSpinSpeed = 0;

    float maxRollSpinSpeed = 360;

    float chargeSpinAccel = 90;

    Vector3 dir;

    Quaternion lastRot;

    float distRolled = 0;

    float maxDistToRoll = 15;

    float curHorzSpeed = 40;

    float maxHorzSpeed = 40;

    float horzDecelleration = 20;
    float spinDecelleration = 180;


    public LayerMask thingsThatCantBeRolledOver;

    public LayerMask player;


    float pangoRadius = 2.12f;

    bool collided = false;

    float restTimer = 0;

    float maxRestTime = 2;

    [HideInInspector]
    public MamaWango mama;

    bool soundPlayed = false;

	// Use this for initialization
	void Start () {


        //FxHandler.inst.PlaySfx("Poof", transform.position);

    }
	
	// Update is called once per frame
	void Update () {

		switch(state)
        {
            case State.Charge:

                LookAtPlayer();

                Spin(curSpinSpeed);
              

                curSpinSpeed += chargeSpinAccel * Time.deltaTime;

                if(curSpinSpeed >= maxRollSpinSpeed)
                {
                    NextState(State.Roll);
                }


                break;
            case State.Roll:

                MoveForward();


                if (!soundPlayed)
                {
                    FxHandler.inst.PlaySfx("Boss_Roll", transform.position);
                    soundPlayed = true;
                }


                if (collided)
                {
                    collided = false;
                    break;
                }

                Spin(curSpinSpeed);


                distRolled += curHorzSpeed * Time.deltaTime;

                if (distRolled >= maxDistToRoll)
                {
                    NextState(State.SlowDown);
                }

                break;
            case State.SlowDown:

                curHorzSpeed -= horzDecelleration * Time.deltaTime;

                MoveForward();

                if (collided)
                {
                    collided = false;
                    break;
                }

                if (curSpinSpeed > 0)
                    curSpinSpeed -= spinDecelleration * Time.deltaTime;
                
                if(curSpinSpeed < 0)
                    curSpinSpeed = 0;

                Spin(curSpinSpeed);

                if(curHorzSpeed <= 0)
                {
                    NextState(State.Rest);
                }

                soundPlayed = false;

                break;
            case State.Rest:

                restTimer += Time.deltaTime;

                if(restTimer >= maxRestTime)
                {
                    NextState(State.Charge);
                }

                break;
            case State.BreakUp:
                break;
        }
	}

    void NextState(State nextState)
    {

        state = nextState;
        switch (nextState)
        {
            case State.Charge:
                curSpinSpeed = 0;
                break;
            case State.Roll:
                distRolled = 0;
                curHorzSpeed = maxHorzSpeed;
                break;
            case State.SlowDown:
                break;
            case State.Rest:
                restTimer = 0;
                break;
            case State.BreakUp:

                // Send signal to mama
                mama.BreakUpBigPango(transform.position);
                FxHandler.inst.PlaySfx("Poof", transform.position);
                FxHandler.inst.PlaySfx("BossSplit", transform.position);

                state = State.Charge;
                curSpinSpeed = 0;

                // Disable self
                gameObject.SetActive(false);
                break;
        }


    }



    void Spin(float spinSpeed)
    {
        transform.GetChild(0).Rotate(Vector3.right, spinSpeed * Time.deltaTime);


    }

    void LookAtPlayer()
    {
        dir = transform.position - PlayerScript.instance.transform.position;

        dir.Normalize();

        Quaternion lookAtRotation = Quaternion.Euler(0, Mathf.Atan2(dir.x, dir.z) * Mathf.Rad2Deg + 180, 0);

        lastRot = Quaternion.RotateTowards(lastRot, lookAtRotation, 150 * Time.deltaTime);//Quaternion.Slerp(lastRot, lookAtRotation, 2 * Time.deltaTime);

        transform.rotation = lastRot;
    }

    void MoveForward()
    {


        // Get a next position
        Vector3 nextPos = transform.position 
                            + transform.rotation 
                                * Vector3.forward 
                                * curHorzSpeed
                                * Time.deltaTime;

        RaycastHit hit;

        if(Physics.SphereCast(transform.position,
                            pangoRadius, 
                            transform.rotation * Vector3.forward, 
                            out hit, 
                            curHorzSpeed * Time.deltaTime, 
                            thingsThatCantBeRolledOver))
        {
            nextPos = transform.position
                            + transform.rotation
                                * Vector3.forward 
                                * hit.distance;

            

            if (hit.collider.gameObject.layer == 15)
            {
                NextState(State.BreakUp);
            }
            else
            {
                NextState(State.Rest);
            }
            collided = true;



        }

        if (Physics.SphereCast(transform.position,
                pangoRadius,
                transform.rotation * Vector3.forward,
                out hit,
                Vector3.Distance(transform.position, nextPos),
                player))
        {
            PlayerHealth.instance.DecreaseHealth(2);
        }

        transform.position = nextPos;


    }



}
