using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MamaWango : MonoBehaviour {

    enum State { Preparation, Singular, Seperated, Dropping}

    State state = State.Preparation;

    State lastState = State.Dropping;

    public List<PangoWango> pangoList;

    int pangosOnPoint = 0;

    public GameObject dustCloud;

    GameObject spawnedDustCloud;


    Vector3 mamaPosition;

    public BigPangoWango bigPango;

    float timer = 0;

    [HideInInspector]
    public int pangosLanded = 0;


    

    public int PangosOnPoint
    {
        get
        {
            return pangosOnPoint;
        }

        set
        {
            if(pangosOnPoint == 0 && value > pangosOnPoint)
            {
                spawnedDustCloud = Instantiate(dustCloud, mamaPosition, Quaternion.identity);
            }
            else if(value == pangoList.Count)
            {
                Destroy(spawnedDustCloud, 1);

                bigPango.transform.position = mamaPosition + Vector3.up * 2;
                bigPango.gameObject.SetActive(true);

            }            
            pangosOnPoint = value;
        }

    }

	// Use this for initialization
	void Start () {
        pangoList = new List<PangoWango>(FindObjectsOfType<PangoWango>());

        foreach (PangoWango pan in pangoList)
            pan.mama = this;

        bigPango = FindObjectOfType<BigPangoWango>();

        bigPango.gameObject.SetActive(false);

        bigPango.mama = this;

        StartBossFight();
	}
	

    public void StartBossFight()
    {
        if (state != State.Preparation) return;

        foreach (PangoWango pan in pangoList)
        {
            pan.NextState(PangoWango.State.Chase);
        }

        NextState(State.Seperated);
    }

	// Update is called once per frame
	void Update () {

        timer += Time.deltaTime;

		switch(state)
        {
            case State.Preparation:
                break;
            case State.Singular:
                break;
            case State.Seperated:
                if (timer > 6)
                {
                    if (lastState != State.Dropping)
                        NextState(State.Dropping);
                    else
                        NextState(State.Singular);
                }
                break;
            case State.Dropping:
                if(pangosLanded == pangoList.Count)
                {
                    NextState(State.Seperated);
                }

                break;
        }
	}

    void NextState(State newState)
    {
        lastState = state;

        timer = 0;
        switch (newState)
        {
            case State.Preparation:
                break;
            case State.Singular:
                FormIntoBigPango();
                break;
            case State.Seperated:
                break;
            case State.Dropping:
                TellAllTheBoisToJump();
                pangosLanded = 0;
                break;
        }
        state = newState;
    }




    void FormIntoBigPango()
    {
        Vector3 center = new Vector3();

        foreach (PangoWango pan in pangoList)
        {
            center.x += pan.transform.position.x;
            center.z += pan.transform.position.z;
        }

        center.x /= pangoList.Count;
        center.y = pangoList[0].transform.position.y;
        center.z /= pangoList.Count;

        mamaPosition = center;


        foreach (PangoWango pan in pangoList)
        {
            pan.MoveToBigMama(center);
        }

    }


    public void BreakUpBigPango(Vector3 bigPangoPos)
    {
        pangosOnPoint = 0;

        foreach(PangoWango pan in pangoList)
        {
            pan.transform.position = bigPangoPos;
            pan.gameObject.SetActive(true);

            pan.NextState(PangoWango.State.Chase);
            NextState(State.Seperated);
        }
    }

    void TellAllTheBoisToJump()
    {
        foreach (PangoWango pan in pangoList)
        {
            pan.NextState(PangoWango.State.JumpAttack);
        }

    }

    public int CheckListPos(PangoWango pango)
    {
        int i = 0;
        foreach(PangoWango pan in pangoList)
        {
            if (pango == pan)
                return i;
            i++;
        }
        return i;
    }

}
