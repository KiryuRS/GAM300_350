using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class PangoWango : MonoBehaviour {


    public enum State { Prepare,Chase, Anticipate, Attack, CoolDown, BigMamaMode, JumpAttack}

    State state = State.Prepare;

    enum JumpState { Anticipate,Jump, Aim, Drop, Wait}

    JumpState jumpState = JumpState.Jump;

    NavMeshAgent nma;

    /// <summary>
    /// Current direction as a vector
    /// </summary>
    Vector3 dir;

    Vector3 draw;
    Vector2 draw2;

    Vector3 draw3;

    Quaternion lastRot;

    public float anticipateTime = 2;
    public float coolDownTime = 2;
    float attackDist = 30;

    Vector3 attackTarget;

    float timer = 0;


    public float chaseSpeed = 10;
    public float attackSpeed = 30;
    public float chaseAccel = 8;
    public float attackAccel = 40;

    Animator anticipationAnim;

    Vector3 bigMamaPosition;

    [HideInInspector]
    public MamaWango mama;


    float jumpStateTimer = 0;

    Animator anim;


    public GameObject dropAimFx;

    GameObject dropAimFx_spawned;

    public LayerMask canDropOntoMask;


    public BoxCollider hitBoxCollider;
    public SphereCollider hitSphereCollider;

    bool jumpAttackExplosion = false;

	// Use this for initialization
	void Start () {
        nma = GetComponent<NavMeshAgent>();

        lastRot = transform.rotation;

        nma.speed = chaseSpeed;
        nma.acceleration = chaseAccel;

        anticipationAnim = transform.GetChild(2).GetComponent<Animator>();

        anim = GetComponent<Animator>();

        hitSphereCollider.enabled = false;

    }
	
	// Update is called once per frame
	void Update () {

        //print(state + "   " + nma.remainingDistance + "        " + nma.pathStatus);

        switch(state)
        {
            case State.Prepare:
                break;
            case State.Chase:
                LookAtPlayer();
                MoveToPlayer();

                if (Vector3.Distance(PlayerScript.instance.transform.position, transform.position) < attackDist * 0.5f)
                {
                    NextState(State.Anticipate);
                }

                break;

            case State.Anticipate:
                LookAtPlayer();                
                nma.SetDestination(transform.position + dir);
                timer -= Time.deltaTime;

                if(timer <= 0)
                {
                    nma.SetDestination(transform.position);
                    NextState(State.Attack);


                    FxHandler.inst.PlaySfx("PangoCharge", transform.position, 0.3f);
                }

                break;

            case State.Attack:

                draw = attackTarget;

                if (nma.remainingDistance < 0.4f * attackDist && nma.speed > 5)
                {
                    //nma.acceleration -= Time.deltaTime * 100;
                    nma.speed -= Time.deltaTime * 50;
                }

                if (nma.remainingDistance < 5 && nma.velocity == Vector3.zero)
                    NextState(State.CoolDown);

                //NavMeshHit hit = new NavMeshHit();

                //if (path.status == NavMeshPathStatus.PathInvalid)
                //{
                //    bool blocked = NavMesh.Raycast(transform.position, attackTarget, out hit, nma.areaMask);

                //    //print(blocked);

                //    attackTarget = hit.position;

                //    nma.SetDestination(attackTarget);
                //}


                break;
            case State.CoolDown:
                LookAtPlayer(true);
                timer -= Time.deltaTime;
                if (timer <= 0)
                    NextState(State.Chase);
                break;


            case State.BigMamaMode:
                if(Vector3.Distance(bigMamaPosition, transform.position) <= 5)
                {
                    mama.PangosOnPoint++;
                    nma.ResetPath();
                    gameObject.SetActive(false);
                }
                break;

            case State.JumpAttack:

                jumpStateTimer += Time.deltaTime;

                switch(jumpState)
                {
                    case JumpState.Anticipate:

                        if(jumpStateTimer > 0.5f)
                        {
                            jumpState = JumpState.Jump;
                        }

                        break;
                    case JumpState.Jump:
                    
                        transform.position += Vector3.up * Time.deltaTime * 40;

                        if (transform.position.y >= 40)
                        {
                            jumpState = JumpState.Aim;

                            RaycastHit hit2;


                            // instantiate aiming thing
                            if (Physics.Raycast(transform.position, Vector3.down, out hit2, 100, canDropOntoMask))
                            {
                                dropAimFx_spawned = Instantiate(dropAimFx, hit2.point, Quaternion.Euler(-90,0,0));
                                jumpStateTimer = 0;

                               
                            }

                            
                            
                        }


                        break;
                    case JumpState.Aim:

                        Vector3 newPos = transform.position;

                        newPos.y = PlayerScript.instance.transform.position.y;

                        newPos = Vector3.Lerp(newPos, PlayerScript.instance.transform.position,  2* Time.deltaTime);

                        newPos.y = transform.position.y;

                        transform.position = newPos;

                        RaycastHit hit;

                        if (Physics.Raycast(transform.position, Vector3.down, out hit, 100, canDropOntoMask))
                        {
                            dropAimFx_spawned.transform.position = hit.point;
                        }

                        if(jumpStateTimer > 2 * (mama.CheckListPos(this) + 1))
                        {
                            jumpState = JumpState.Drop;


                            hitBoxCollider.enabled = false;

                            
                        }


                        break;
                    case JumpState.Drop:

                        Vector3 newPos2 = transform.position + Vector3.down * Time.deltaTime * 40;

                        RaycastHit hit3;

                        if(Physics.Raycast(transform.position, Vector3.down, out hit3, Time.deltaTime * 40, canDropOntoMask))
                        {
                            newPos2 = hit3.point;
                            anim.SetTrigger("Jump");
                            jumpState = JumpState.Wait;

                            Destroy(dropAimFx_spawned);
                            jumpStateTimer = 0;
                            mama.pangosLanded++;
                            nma.updatePosition = true;

                            FxHandler.inst.PlayPfx("Big SmokePfx", newPos2);
                            FxHandler.inst.PlaySfx("Smash", newPos2);

                            hitSphereCollider.enabled = true;
                            jumpAttackExplosion = true;
                        }

                        transform.position = newPos2;

                        break;
                    case JumpState.Wait:
                        if (jumpAttackExplosion)
                            jumpAttackExplosion = false;

                        if(!hitBoxCollider.gameObject.activeInHierarchy)
                            hitBoxCollider.enabled = true;
                        if (hitSphereCollider.gameObject.activeInHierarchy)
                            hitSphereCollider.enabled = false;
                        break;
                }
                break;

        }
           
    }

    NavMeshPath path;

    public void NextState(State nextState)
    {
        switch(nextState)
        {
            case State.Chase:
                nma.speed = chaseSpeed;
                nma.acceleration = chaseAccel;

                nma.stoppingDistance = 0;

                break;

            case State.Anticipate:
                timer = 2;
                //transform.GetChild(1).gameObject.SetActive(true);
                anticipationAnim.SetTrigger("Anticipate");


            
                break;

            case State.Attack:
                transform.GetChild(1).gameObject.SetActive(false);
                transform.GetChild(0).gameObject.SetActive(true);

                attackTarget = transform.position - dir * attackDist;

         

                path = new NavMeshPath();
                nma.CalculatePath(attackTarget, path);


                NavMeshHit hit = new NavMeshHit();

                if(path.status == NavMeshPathStatus.PathInvalid )
                {
                    bool blocked = NavMesh.Raycast(transform.position, attackTarget, out hit, nma.areaMask);

                    //print(blocked);

                    attackTarget = hit.position;

                    draw3 = hit.position;
                }

                nma.SetDestination(attackTarget);
               
                nma.speed = attackSpeed;
                nma.acceleration = attackAccel;

                nma.stoppingDistance = 0;

                

                break;

            case State.CoolDown:

                transform.GetChild(0).gameObject.SetActive(false);
                timer = coolDownTime;
                break;

            case State.BigMamaMode:
                ResetSpeeds();
                FxHandler.inst.PlaySfx("Poof", transform.position, 0.3f);
                break;

            case State.JumpAttack:
                ResetSpeeds();
                jumpStateTimer = 0;
                jumpState  = JumpState.Anticipate;

                FxHandler.inst.PlaySfx("PangoJump", transform.position, 0.2f);

                anim.SetTrigger("Jump");
                nma.updatePosition = false;

                break;

        }
        state = nextState;
    }


    void ResetSpeeds()
    {
        transform.GetChild(1).gameObject.SetActive(false);
        transform.GetChild(0).gameObject.SetActive(false);

        nma.ResetPath();

        nma.speed = chaseSpeed;
        nma.acceleration = chaseAccel;
    }


    void LookAtPlayer(bool slowTurn = false)
    {
        dir = transform.position - PlayerScript.instance.transform.position;

        dir.Normalize();

        Quaternion lookAtRotation = Quaternion.Euler(0, Mathf.Atan2(dir.x, dir.z) * Mathf.Rad2Deg + 180, 0);

        lastRot = Quaternion.RotateTowards(lastRot, lookAtRotation, 150 * Time.deltaTime);//Quaternion.Slerp(lastRot, lookAtRotation, 2 * Time.deltaTime);

        transform.rotation = lastRot;

        draw2 = dir;
    }

    void MoveToPlayer()
    {
        nma.SetDestination(PlayerScript.instance.transform.position + dir * 3);

        draw = PlayerScript.instance.transform.position + dir * 2;

    }


    Vector3 drawImpulseDirection;
    private void OnDrawGizmos()
    {
        Gizmos.DrawSphere(draw, 1);
        Gizmos.DrawLine(transform.position + Vector3.up * 3, transform.position + (Vector3)draw2 + Vector3.up * 3);

        Gizmos.color = Color.red;
        Gizmos.DrawSphere(draw3, 0.5f);

        
        Gizmos.DrawLine(transform.position, transform.position + drawImpulseDirection * 10);

    }


    //private void OnCollisionEnter(Collision collision)
    //{
    //    print(collision.gameObject.tag);
    //    if (collision.gameObject.tag == "Player")
    //    {
    //        if (state == State.Attack)
    //        {
    //            PlayerHealth.instance.DecreaseHealth(1);
    //
    //            Vector3 impulseDir = transform.position + PlayerScript.instance.transform.position;
    //
    //            impulseDir.Normalize();
    //
    //            //PlayerScript.instance.GetPushedBack(impulseDir * 20);
    //        }
    //        else if (state == State.JumpAttack && jumpState == JumpState.Drop)
    //        {
    //            PlayerHealth.instance.DecreaseHealth(1);
    //        }
    //    }
    //}

    private void OnTriggerEnter(Collider other)
    {
        if(other.gameObject.tag == "Player")
            if (state == State.Attack)
            {
                PlayerHealth.instance.DecreaseHealth(1);

                Vector3 impulseDir = (transform.position - PlayerScript.instance.transform.position) * -1;

                impulseDir.y = 0;

                impulseDir.Normalize();

                drawImpulseDirection = impulseDir;

                PlayerScript.instance.GetPushedBack(impulseDir * 20);
            }
            //else if(state == State.JumpAttack && jumpState == JumpState.Drop)
            //{

            //}
    }

    private void OnTriggerStay(Collider other)
    {
        if (jumpAttackExplosion)
        {
            if (other.gameObject.tag == "Player")
            {
                jumpAttackExplosion = false;
                PlayerHealth.instance.DecreaseHealth(1);


                Vector3 impulseDir = (transform.position - PlayerScript.instance.transform.position) * -1;

                impulseDir.y = 0;

                if (impulseDir.magnitude == 0)
                {
                    impulseDir = Random.insideUnitSphere;
                    impulseDir.y = 0;
                }

                impulseDir.Normalize();

                drawImpulseDirection = impulseDir;

                PlayerScript.instance.GetPushedBack(impulseDir * 10);
            }
        }
        

    }


    public void MoveToBigMama(Vector3 mamaPosition)
    {
        NextState(State.BigMamaMode);
        nma.SetDestination(mamaPosition);

        bigMamaPosition = mamaPosition;
    }


    


}
