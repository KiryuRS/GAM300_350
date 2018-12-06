using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StoneIndiBehaviour : MonoBehaviour
{
  public int State;
  public float Timer;
  public float IdleLimit;
  public GameObject Player;
  public float AttackCooldown;
  float InnerAttackCooldown;
  public float AttackRangeLimit, AttackMovementSpeed;
  public Vector3 Target, TargetDir, TargetEuler;
  public float LerpTimer;
  public float sizeY;
  public float RotationForce;
  public List<GameObject> BodyPartsToHide;
  public float AttackTimerLimit;
  public GameObject DropZoneTemplate;
  public LayerMask DropZoneLM;
  public float RigidbodySpeedLimit;
  Vector3 LerpStart;
  GameObject DropZone;


  public GameObject dropParticle;
  public GameObject darkAura;
  public GameObject electricParticle;
    public GameObject fireParticle;

    bool particleSpawned = false;
    GameObject fireParticleObj;

    float indiChargeSpeed = 0.6f;

  // Use this for initialization
  void Start()
  {
    Target = Vector3.zero;
    LerpStart = Vector3.zero;
    sizeY = 0;
    DropZone = null;
  }

  public void InitializeStone()
  {
    Start();
  }

  // Update is called once per frame
  void Update()
  {
    if (GetComponent<Rigidbody>().velocity.magnitude > RigidbodySpeedLimit)
    {
      GetComponent<Rigidbody>().velocity = GetComponent<Rigidbody>().velocity.normalized * RigidbodySpeedLimit;
    }
    //Quaternion curRot = transform.rotation;
    //float diff = 0;
    //if (curRot.eulerAngles.x != 0)
    //{
    //    diff = 0 - curRot.eulerAngles.x;
    //    diff /= Mathf.Abs(diff);
    //    transform.Rotate(new Vector3(1, 0, 0), diff * RotationForce * Time.deltaTime);
    //}
    //if (curRot.eulerAngles.y != 0)
    //{
    //    diff = 0 - curRot.eulerAngles.y;
    //    diff /= Mathf.Abs(diff);
    //    transform.Rotate(new Vector3(0, 1, 0), diff * RotationForce * Time.deltaTime);
    //}
    //if (curRot.eulerAngles.z != 0)
    //{
    //    diff = 0 - curRot.eulerAngles.z;
    //    diff /= Mathf.Abs(diff);
    //    transform.Rotate(new Vector3(0, 0, 1), diff * RotationForce * Time.deltaTime);
    //}

    if (InnerAttackCooldown > 0)
      InnerAttackCooldown -= Time.deltaTime;

    Quaternion curRot;
    switch (State)
    {
      case 0://receive command
        curRot = transform.rotation;
        if (curRot.eulerAngles.x != 0)
          transform.Rotate(-curRot.eulerAngles.x, 0, 0);
        if (curRot.eulerAngles.z != 0)
          transform.Rotate(0, 0, -curRot.eulerAngles.z);
        break;
      case 1: //idle
        if (InnerAttackCooldown <= 0)
        {
          Timer += Time.deltaTime;
          if (Timer >= IdleLimit)
            Timer = IdleLimit = State = 0;
        }
        break;
      case 2: //attack player
        if (Target == Vector3.zero)
        {
          Target = Player.transform.position;
          TargetDir = Target - transform.position;
          TargetDir.Normalize();
        }

                if (indiChargeSpeed < 3)
                    indiChargeSpeed += Time.deltaTime *1.3f;

        transform.LookAt(Target);
        Vector3 curDir = Target - transform.position;
        transform.position += curDir.normalized * AttackMovementSpeed * indiChargeSpeed * Time.deltaTime;
                //print(Vector3.Dot(curDir.normalized, TargetDir));

              
                if(!particleSpawned)
                {
                    fireParticleObj = Instantiate(fireParticle, transform.position, transform.rotation);
                    fireParticleObj.transform.SetParent(gameObject.transform);
                    fireParticleObj.transform.localRotation = Quaternion.Euler(0, 180, 0);
                    particleSpawned = true;
                }

                

        // DAMAGE
        if (Vector3.Distance(Player.transform.position, transform.position) < 2)
        {
                    // Check if player is invulnerable first
                    if (!PlayerHealth.instance.invul)
                    {
                        PlayerHealth.instance.DecreaseHealth(1);



                        FxHandler.inst.PlayPfx("CFX2_SparksHit_B Sphere", Player.transform.position);
                        FxHandler.inst.PlayPfx("CFX2_RockHit", Player.transform.position);

                        Target = Vector3.zero;
                        State = 0;
                        InnerAttackCooldown = AttackCooldown;
                        Timer = 0;

                        Destroy(fireParticleObj);
                        particleSpawned = false;

                        indiChargeSpeed = 0.6f;

                        //<< TURN OFF FLAME FX
                        break;
                    }
        }

        Timer += Time.deltaTime;
        if ((curDir).magnitude < AttackRangeLimit ||
           Vector3.Dot(curDir.normalized, TargetDir) < 0 ||
           Timer >= AttackTimerLimit)
        {
          GetComponent<Rigidbody>().AddForce(curDir.normalized * AttackMovementSpeed * 5);
          Target = Vector3.zero;
          State = 0;
          InnerAttackCooldown = AttackCooldown;
          Timer = 0;

                    indiChargeSpeed = 0.6f;


                    Destroy(fireParticleObj);
                    particleSpawned = false;
                    //<< TURN OFF FLAME FX
                }
                break;
      case 3: //jumping up
        transform.position += new Vector3(0, 5, 0) * AttackMovementSpeed * Time.deltaTime;
        if (transform.localScale.y != sizeY)
          transform.localScale = new Vector3(transform.localScale.x, sizeY, transform.localScale.z);
        break;
      case 4: //dropping down
        if (transform.position.x != Target.x &&
            transform.position.z != Target.z)
          transform.position = new Vector3(Target.x, transform.position.y, Target.z);
        Vector3 Dir = Target - transform.position;
        transform.position += Dir.normalized * AttackMovementSpeed * 3.0f * Time.deltaTime;
        DropZone.transform.localScale = new Vector3(DropZone.transform.localScale.x - 0.1f * Time.deltaTime, DropZone.transform.localScale.y - 0.1f * Time.deltaTime, DropZone.transform.localScale.z);
        if ((Dir).magnitude < AttackRangeLimit)
        {
          Target = Vector3.zero;
          State = 0;
          sizeY = 0;
          Destroy(DropZone);

          Instantiate(dropParticle, transform.position, Quaternion.identity);

                    FxHandler.inst.PlaySfx("Smash", transform.position);
          
          // DAMAGE
          if (Vector3.Distance(Player.transform.position, transform.position) < 4)
          {
                        if (!PlayerHealth.instance.invul)
                        {
                            PlayerHealth.instance.DecreaseHealth(1);

                            FxHandler.inst.PlayPfx("CFX2_SparksHit_B Sphere", Player.transform.position);
                            FxHandler.inst.PlayPfx("CFX2_RockHit", Player.transform.position);
                        }
          }

        }


        break;
      case 5://forming for spin
        if (LerpStart == Vector3.zero)
          LerpStart = transform.position;
        transform.position = Vector3.Lerp(LerpStart, Target, LerpTimer);
        LerpTimer += Time.deltaTime;
        transform.eulerAngles = TargetEuler;
        if (LerpTimer >= 1)
        {

                    transform.position = Target;
          LerpStart = Vector3.zero;
          State = 6;
          foreach (GameObject GO in BodyPartsToHide)
            GO.SetActive(false);

          Instantiate(electricParticle, transform.position, Quaternion.identity);

                    FxHandler.inst.PlaySfx("BossSpin", transform.position, 1.5f);
                }
        break;
      case 6://spinning
        break;
      case 7://drop charge
        curRot = transform.rotation;
        if (curRot.eulerAngles.x != 0)
          transform.Rotate(-curRot.eulerAngles.x, 0, 0);
        //transform.rotation.eulerAngles = new Vector3(0, curRot.eulerAngles.y, curRot.eulerAngles.z);
        if (curRot.eulerAngles.z != 0)
          transform.Rotate(0, 0, -curRot.eulerAngles.z);
        //transform.rotation.eulerAngles = new Vector3(curRot.eulerAngles.x, curRot.eulerAngles.y, 0);
        if (sizeY == 0)
          sizeY = transform.localScale.y;
        if (transform.localScale.y >= sizeY / 2)
        {
          transform.localScale = new Vector3(transform.localScale.x, transform.localScale.y - 1.0f * Time.deltaTime, transform.localScale.z);
        }
        break;
      default: break;
    }
  }

  public void ShowDropZone()
  {
    if (!DropZone)
    {
      RaycastHit hit;
      if (Physics.Raycast(transform.position, (Target - transform.position), out hit, DropZoneLM))
      {
        // DropZone = GameObject.Instantiate(DropZoneTemplate, Target, Quaternion.Euler(90, 0, 0));

        DropZone = Instantiate(darkAura, Target, Quaternion.Euler(-90, 0, 0));
      }

    }
  }

  public void Unhide()
  {
    foreach (GameObject GO in BodyPartsToHide)
      GO.SetActive(true);
  }

  public void DealtDamage(int damage)
  {
    BossHealthBar.instance.DecreaseHealth(damage);
    //FxHandler.inst.PlayPfx("CFX3_Hit_Misc_D", transform.position);
    //FxHandler.inst.PlayPfx("CFX3_Fire_Explosion", transform.position);
    //FxHandler.inst.PlayPfx("CFX2_RockHit", transform.position);
    FxHandler.inst.PlayPfx("CFX4 Magic Hit", transform.position);
  }


}
