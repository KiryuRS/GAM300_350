using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FiveStoneBoss : MonoBehaviour
{

    public List<GameObject> Stones;
    public GameObject Player;
    public GameObject Ground;
    public float IdleLimit, AttackLimit;
    public float AttackMovementSpeed;
    public float AttackCooldown;
    public float DropTimer;
    public float DropCooldown, DropBaseCooldown;
    public float SpinCooldown, SpinBaseCooldown;
    public float GeneralCooldown;
    public float DropChargeTime, SpinDelayTime;
    public float SpinSpeed = 10.0f;
    public float DropdownHeight;

    public bool SpinMode, SpinPrep, Spinning, DropMode, DropCharge, Dropping;
    float Timer;
    int StoneIndex;
    Vector3 SpinStart, SpinEnd;
    Quaternion SpinStartRot, SpinEndRot;


    [Header("Hp Properties")]
    public int maxHp = 10;

    public GameObject stunParticle;



    // Use this for initialization
    void Start()
    {
        if (!Ground)
            Ground = GameObject.FindGameObjectWithTag("Ground");
        if (!Player)
            Player = GameObject.FindGameObjectWithTag("Player");
        if (Stones.Count == 0)
            for (int i = 0; i < transform.childCount; ++i)
            {
                Stones.Add(transform.GetChild(i).gameObject);

                StoneIndiBehaviour stone = transform.GetChild(i).GetComponent<StoneIndiBehaviour>();
                stone.InitializeStone();

                stone.Player = Player;
                stone.AttackRangeLimit = AttackLimit;
            }
        transform.DetachChildren();


    }

    public void Spawn()
    {
        Start();


    }

    // Update is called once per frame
    void Update()
    {
        if (GeneralCooldown > 0)
        {
            GeneralCooldown -= Time.deltaTime;
            return;
        }
        int state = Random.Range(0, 5);
        if (DropCooldown > 0)
            DropCooldown -= Time.deltaTime;
        if (SpinCooldown > 0)
            SpinCooldown -= Time.deltaTime;
        if (state == 3 && !DropMode && !Dropping && DropCooldown <= 0 && !SpinMode)
            DropMode = true;
        if (state == 4 && !SpinMode && !Spinning && SpinCooldown <= 0 && !DropMode)
        {
            transform.position = Stones[0].transform.position;
            transform.Translate(0, Stones[0].transform.localScale.y, 0, Ground.transform);
            SpinMode = true;
        }
        foreach (GameObject GO in Stones)
        {
            if (GO.GetComponent<StoneIndiBehaviour>().State == 0)
            {
                if (DropMode || SpinMode)
                {
                    break;
                }
                else
                {
                    StoneIndiBehaviour SB = GO.GetComponent<StoneIndiBehaviour>();
                    switch (state)
                    {
                        case 0:
                            SB.State = 1;
                            SB.IdleLimit = IdleLimit;
                            SB.AttackCooldown = AttackCooldown;
                            break;
                        default:
                            SB.State = 2;
                            SB.AttackMovementSpeed = AttackMovementSpeed;
                            break;
                    }
                }
            }
        }
        if (SpinMode && !SpinPrep)
        {
            int count = 0;
            foreach (GameObject GO in Stones)
                if (GO.GetComponent<StoneIndiBehaviour>().State == 0)
                    ++count;
            if (count == 5)
            {
                transform.position = Stones[0].transform.position;
                transform.Translate(0, Stones[0].transform.localScale.y, 0, Ground.transform);
                SpinPrep = true;
                StoneIndex = 0;
                transform.LookAt(Player.transform.position);
                Vector3 relativePos = Player.transform.position - transform.position;
                Quaternion rotation = Quaternion.LookRotation(relativePos);
                float StoneInterver = 72.0f / 180.0f * Mathf.PI;
                foreach (GameObject GO in Stones)
                {
                    float dist = GO.transform.localScale.y / 2;
                    GO.GetComponent<StoneIndiBehaviour>().AttackMovementSpeed = AttackMovementSpeed;
                    GO.GetComponent<StoneIndiBehaviour>().State = 5;
                    Vector3 targetPos = transform.position + new Vector3(Mathf.Cos(StoneInterver * StoneIndex) * dist * Mathf.Sin(rotation.eulerAngles.y / 180.0f * Mathf.PI),
                        Mathf.Sin(StoneInterver * StoneIndex) * dist,
                        Mathf.Cos(StoneInterver * StoneIndex) * Mathf.Cos(rotation.eulerAngles.y / 180.0f * Mathf.PI) * dist);
                    //print(new Vector3(Mathf.Cos(rotation.eulerAngles.y * 72.0f / 180.0f * Mathf.PI * StoneIndex), Mathf.Sin(rotation.eulerAngles.y * 72.0f / 180.0f * Mathf.PI * StoneIndex)));
                    //print(rotation.eulerAngles.y);
                    GO.GetComponent<StoneIndiBehaviour>().Target = targetPos;
                    float rotationAngle = StoneIndex * -72.0f + 90.0f;
                    if (rotationAngle >= 360.0f)
                        rotationAngle -= 360.0f;
                    GO.GetComponent<StoneIndiBehaviour>().TargetEuler = new Vector3(rotationAngle, rotation.eulerAngles.y, 0);
                    GO.GetComponent<BoxCollider>().enabled = false;
                    GO.GetComponent<Rigidbody>().velocity = Vector3.zero;
                    GO.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
                    GO.GetComponent<Rigidbody>().useGravity = false;
                    GO.GetComponent<Rigidbody>().freezeRotation = true;
                    ++StoneIndex;
                }
            }
        }
        if (SpinPrep && !Spinning)
        {
            int count = 0;
            foreach (GameObject GO in Stones)
                if (GO.GetComponent<StoneIndiBehaviour>().State == 6)
                    ++count;
            if (count == 5)
            {
                Spinning = true;
                GetComponent<Rigidbody>().useGravity = true;
                GetComponent<SphereCollider>().enabled = true;
                SpinStart = transform.position;
                //SpinStart.y = 0;

                Vector3 playerPos = Player.transform.position;
                playerPos.y = transform.position.y;

                Vector3 SpinDir = playerPos - SpinStart;
                SpinEnd = SpinStart + SpinDir * 2;
                //SpinEnd.y = 0;
                foreach (GameObject GO in Stones)
                    GO.transform.parent = transform;
                Timer = 0;
                transform.LookAt(Player.transform);

            }
        }
        if (Spinning)
        {
            transform.Rotate(new Vector3(1.0f, 0.0f, 0.0f), SpinSpeed);
            if (Timer >= SpinDelayTime)
            {
                transform.position += (SpinEnd - SpinStart) * Time.deltaTime;
                Vector3 SpinDir = SpinEnd - SpinStart;
                SpinDir.Normalize();
                if (Vector3.Dot(SpinEnd - transform.position, SpinDir) < 0)
                    SpinBreak();
            }
            else
            {
                //print(transform.eulerAngles.y);
                Timer += Time.deltaTime;
                //transform.LookAt(Player.transform.position);
            }
        }
        if (DropMode && !DropCharge)
        {
            int count = 0;
            foreach (GameObject GO in Stones)
                if (GO.GetComponent<StoneIndiBehaviour>().State == 0)
                    ++count;
            if (count == 5)
            {
                DropCharge = true;
                foreach (GameObject GO in Stones)
                {
                    //GO.GetComponent<StoneIndiBehaviour>().AttackMovementSpeed = AttackMovementSpeed;
                    GO.GetComponent<StoneIndiBehaviour>().State = 7;
                }
                Timer = 0;
                StoneIndex = 0;
            }
        }
        if (DropCharge && !Dropping)
        {
            Timer += Time.deltaTime;
            if (Timer >= DropChargeTime)
            {
                Dropping = true;
                foreach (GameObject GO in Stones)
                {
                    GO.GetComponent<StoneIndiBehaviour>().AttackMovementSpeed = AttackMovementSpeed;
                    GO.GetComponent<StoneIndiBehaviour>().State = 3;

                }
                Timer = -1;
            }
        }
        if (Dropping)
        {
            if (StoneIndex != 5)
            {
                if (Timer >= DropTimer)
                {
                    Timer = 0;
                    Stones[StoneIndex].GetComponent<StoneIndiBehaviour>().State = 4;
                    Stones[StoneIndex].GetComponent<StoneIndiBehaviour>().Target = Player.transform.position;
                    Stones[StoneIndex].transform.position = new Vector3(Stones[StoneIndex].GetComponent<StoneIndiBehaviour>().Target.x, DropdownHeight, Stones[StoneIndex].GetComponent<StoneIndiBehaviour>().Target.z);
                    Stones[StoneIndex].GetComponent<StoneIndiBehaviour>().ShowDropZone();
                    ++StoneIndex;

                }
                else
                    Timer += Time.deltaTime;
            }
            else
            {
                DropCooldown = DropBaseCooldown;
                Dropping = DropMode = DropCharge = false;
                GeneralCooldown = 6;
            }
        }
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (Spinning && collision.transform.tag != "Ground")
        {
            if (collision.gameObject.layer == 15)
            {
                SpinBreak(true);

                FxHandler.inst.PlayPfx("CFX2_RockHit", transform.position);

            }
            else if (collision.transform.tag == "Player")
            {
                PlayerHealth.instance.DecreaseHealth(1);
                SpinBreak();
                FxHandler.inst.PlayPfx("CFX3_Hit_Electric_A_Ground", collision.contacts[0].point);
                FxHandler.inst.PlayPfx("CFX3_Fire_Explosion", collision.contacts[0].point);

            }
            else
            {
                //individual stones get stunned
                SpinBreak();
            }
        }


    }

    void SpinBreak(bool hit = false)
    {
        Timer = 0;
        Spinning = SpinPrep = SpinMode = false;
        GetComponent<Rigidbody>().useGravity = false;
        GetComponent<SphereCollider>().enabled = false;
        GetComponent<Rigidbody>().velocity = Vector3.zero;
        foreach (GameObject GO in Stones)
        {
            GO.GetComponent<StoneIndiBehaviour>().State = 0;
            GO.GetComponent<Rigidbody>().velocity = Vector3.zero;
            GO.GetComponent<Rigidbody>().useGravity = true;
            GO.GetComponent<BoxCollider>().enabled = true;
            GO.GetComponent<StoneIndiBehaviour>().LerpTimer = 0;
            GO.GetComponent<Rigidbody>().freezeRotation = false;
            GO.GetComponent<StoneIndiBehaviour>().Unhide();
            ++StoneIndex;

            if (hit)
            {
                Vector3 spawnPos = new Vector3(GO.transform.position.x, GO.transform.position.y, GO.transform.position.z);
                GameObject particle = Instantiate(stunParticle, spawnPos, GO.transform.rotation);
                particle.transform.SetParent(GO.transform);

                particle.transform.localPosition = new Vector3(0, 1, 0);

                Destroy(particle, 8);
            }
        }

        if (hit)
            GeneralCooldown = 8;
        else
            GeneralCooldown = 1;

        SpinCooldown = SpinBaseCooldown;
        //transform.localRotation = Quaternion.identity;
        transform.DetachChildren();
    }

    public void DealtDamage(int damage)
    {
        BossHealthBar.instance.DecreaseHealth(damage);

        FxHandler.inst.PlayPfx("CFX4 Magic Hit", transform.position);
    }

    public void DeathScene()
    {
        StartCoroutine(Deaded());
    }

    IEnumerator Deaded()
    {
        Vector3 squashedScale = new Vector3(1, 3, 1);

        float time = 0;

        while (time < 3)
        {
            foreach (GameObject GO in Stones)
            {
                GO.transform.localScale = Vector3.Lerp(GO.transform.localScale, squashedScale, Time.deltaTime * 10);
            }
            yield return null;
            time += Time.deltaTime;
        }
    }
}
