using UnityEngine;
using System.Collections;
using System.Collections.Generic;
// using Holoville.HOTween;

public class PlayerController : MonoBehaviour 
{
	public static PlayerController instance;

	public GameObject cameraFocalPoint;

	public bool canCapture;
	public GameObject objCaptured;
	public bool isPulling;
	public bool killPull;
	public SpringJoint2D pullJoint;
	private float pullingForce = 0.1f; 

	private float currentMoveModifier = 1;

	public Vector3 aimingWorldPos;
	public Vector3 controllerAimingOffset;
	private Vector3 lookingDirection;

	public Vector2 lastVelocity;

	public bool checkingTripping;

	private bool canBeStunned = false;
	public bool isStunned = false;
	public GameObject stunner;

	private static int currentHealth = 3;
	public GameObject[] heartContainers;
	public GameObject heartFab;

	private bool canBeDamaged = true;
	private bool isAlive = true;
	private bool killed = false;
	public GameObject deathParticleSystem;

	public LineRenderer pullLine;

	public float totalPullStrength;
	public float currentPullStrength;

	public bool useController;

	public bool mouseMove;

	public bool inItemSelect;

	public List<Item> inventory = new List<Item>();

	public ItemButton actionNorth;
	public ItemButton actionEast;
	public ItemButton actionSouth;
	public ItemButton actionWest;


	void Start () 
	{
		instance = this;
		useController = false;

		for (int index = 0;
		     index < heartContainers.Length;
		     index++)
		{
			heartContainers[index].SetActive(true);
			if (index > currentHealth - 1)
			{
				heartContainers[index].SetActive(false);
			}
		}
	}

	void Update () 
	{
		if (Input.GetButtonDown("Menu"))
		{
			God.instance.isPaused = !God.instance.isPaused;

			if (ItemSelectionController.instance.inItemSelection)
			{
				ItemSelectionController.instance.CloseItemSelection();
				ItemSelectionController.instance.inItemSelection = false;
			}
			else
			{
				ItemSelectionController.instance.OpenItemSelection();
				ItemSelectionController.instance.inItemSelection = true;
			}
		}

		if (!God.instance.isPaused)
		{
			if (Input.GetButtonDown("ActionNorth"))
			{
				actionNorth.Use();
			}
			if (Input.GetButtonDown("ActionEast"))
			{
				actionEast.Use();
			}
			if (Input.GetButtonDown("ActionSouth"))
			{
				actionSouth.Use();
			}
			if (Input.GetButtonDown("ActionWest"))
			{
				actionWest.Use();
			}


			if (Input.GetMouseButtonDown(0))
			{
				useController = false;
			}
			if (Input.GetAxis("Horizontal") > 0 || Input.GetAxis("Vertical") > 0)
			{
				useController = true;
			}

			currentPullStrength = Mathf.Clamp(currentPullStrength, 0, 99);

			if (isPulling)
			{
				currentPullStrength -= 0.5f;
			}
			else
			{
				currentPullStrength += 1;
			}

			if (currentPullStrength <= 0)
			{
				KillPull();
				Stun();
			}

			if (!isStunned && isAlive)
			{
				if (useController)
				{
					if (Input.GetButton("ControllerShooting"))
					{
						if (objCaptured != null && !isPulling)
						{
							objCaptured.GetComponent<EnemyController>().RunFromCapture();
							StartPull();
						}
					}
					else
					{
						if (isPulling)
						{
							KillPull();
						}
					}

					if (isPulling)
					{
						currentMoveModifier = 0.4f;

						if (!checkingTripping)
						{
							CheckTrip();
						}
					}
					else
					{
						currentMoveModifier = 1;
					}

					if (objCaptured != null && !isPulling)
					{
						if (Vector3.Distance(objCaptured.transform.position, this.transform.position) > 2.5)
						{
							objCaptured = null;
						}
					}

					float horizontalChange = Input.GetAxis("Horizontal") * Time.deltaTime * currentMoveModifier * 10;
					float verticalChange = Input.GetAxis("Vertical") * Time.deltaTime * currentMoveModifier * 10;
					Vector3 newPos = new Vector3(this.transform.position.x + horizontalChange, 
					                             this.transform.position.y + verticalChange, 
					                             0);
					this.transform.position = newPos;

					Vector3 movingDirection = new Vector3(Input.GetAxis("Horizontal"), Input.GetAxis("Vertical"), 0);
					movingDirection.Normalize();
					movingDirection = (movingDirection * 15) + this.transform.position;
					Utility.PointAt(this.gameObject, movingDirection);
				}
			}

			if (currentHealth <= 0)
			{
				if (!killed)
				{
					killed = true;	
					Instantiate(deathParticleSystem, this.transform.position, Quaternion.identity);
					this.gameObject.SetActive(false);
				}
				isAlive = false;
			}
		}
	}

	public void StartPull()
	{
		isPulling = true;
		killPull = false;
		pullJoint.distance = 1.5f;

		pullLine.enabled = true;

		this.pullJoint.enabled = true;
		this.pullJoint.connectedBody = objCaptured.GetComponent<Rigidbody2D>();

		StartCoroutine(CapturePull_());
	}
	private IEnumerator CapturePull_()
	{
		Rigidbody2D body = objCaptured.GetComponent<Rigidbody2D>() as Rigidbody2D;
		while (isPulling)
		{
			pullLine.SetPosition(0, this.transform.position);
			pullLine.SetPosition(1, objCaptured.transform.position);

			body.AddForce(objCaptured.transform.up * pullingForce * 10);
			pullJoint.distance -= 0.001f * pullingForce;
			Utility.PointAt(objCaptured, this.transform.position);
			objCaptured.GetComponent<EnemyController>().currentHealth -= 0.5f;

			if (!objCaptured.GetComponent<EnemyController>().alive)
			{
				KillPull();
			}

			yield return new WaitForSeconds(0.01f);
		}

		if (!killPull)
		{
			pullLine.enabled = false;
			isPulling = false;
			this.pullJoint.connectedBody = null;
			this.pullJoint.enabled = false;

			objCaptured.GetComponent<EnemyController>().isRunning = false;

			this.GetComponent<Rigidbody2D>().isKinematic = true;
			this.GetComponent<Rigidbody2D>().isKinematic = false;
		}
	}

	public void CheckTrip()
	{
		checkingTripping = true;
		canBeStunned = false;
		ResetCanBeStunned();
		StartCoroutine(CheckTrip_());
	}
	public IEnumerator CheckTrip_()
	{
		Vector3 hopePoint = objCaptured.transform.position - (objCaptured.transform.up * pullingForce * 10);		
		float firstDistance = Vector3.Distance(hopePoint, this.transform.position);

		yield return new WaitForSeconds(0.1f);

		float secondDistance = Vector3.Distance(hopePoint, this.transform.position);
		if (((secondDistance + 0.1f) < firstDistance) && canBeStunned)
		{
			KillPull();
			Stun();
		}

		checkingTripping = false;
	}

	public void KillPull()
	{
		objCaptured.GetComponent<EnemyController>().isRunning = false;
		this.pullJoint.enabled = false;
		pullLine.enabled = false;
		isPulling = false;
		killPull = true;

		this.GetComponent<Rigidbody2D>().isKinematic = true;
		this.GetComponent<Rigidbody2D>().isKinematic = false;
	}

	public void Stun()
	{
		stunner.SetActive(true);
		isStunned = true;
		isPulling = false;
		StartCoroutine(Stun_());
	}
	public IEnumerator Stun_()
	{
		yield return new WaitForSeconds(1f);
		isStunned = false;
		stunner.SetActive(false);
	}

	void OnCollisionEnter2D(Collision2D coll)
	{
		Chest chest = coll.gameObject.GetComponent<Chest>();
		if (chest && !chest.open)
		{
			chest.Open();
			return;
		}

		Item item = coll.gameObject.GetComponent<Item>();
		if (item)
		{
			item.itemHeld = true;
			bool stacking = false;

			for (int index = 0;
			     index < inventory.Count;
			     index++)
			{
				if (inventory[index].itemName == item.itemName)
				{
					stacking = true;
					inventory[index].stackCount++;
					Destroy(item.gameObject);
					break;
				}
			}

			if (!stacking)
			{
				inventory.Add(item);
				coll.gameObject.transform.parent = this.transform;
				coll.gameObject.transform.position = new Vector3(1000, 1000, 1000);
			}
			return;
		}

		TutorialLadder tutLadder = coll.gameObject.GetComponent<TutorialLadder>();
		if (tutLadder)
		{
			if (tutLadder.canEnter)
			{
				Application.LoadLevel(tutLadder.GetComponent<TutorialLadder>().sceneIndexTo);
			}
			return;
		}

		LadderController ladderCont = coll.gameObject.GetComponent<LadderController>();
		if (ladderCont)
		{
			// Turn this one to allow only gonig through ladders when all the enemies are saved. 
			// if (ladderCont.canEnter)
			// {
			Application.LoadLevel(Application.loadedLevel);
			// }
			return;
		}

		if ((coll.gameObject.GetComponent<EnemyController>() && canBeDamaged && isAlive) || 
		    (coll.gameObject.GetComponent<BulletController>()))
		{
			TakeDamage();

			if (coll.gameObject.GetComponent<BulletController>())
			{
				Destroy(coll.gameObject);
			}

			return;
		}

		if (coll.gameObject.GetComponent<BulletController>())
		{
			Destroy(coll.gameObject);
			return;
		}

		if (coll.gameObject.GetComponent<HeartPickup>())
		{
			Destroy(coll.gameObject);
			heartContainers[currentHealth].SetActive(true);
			currentHealth++;
			return;
		}
	}

	void OnTriggerEnter2D(Collider2D coll)
	{
		if (WorldGenerator.instance != null)
		{
			if (!WorldGenerator.instance.isGenerating)
			{
				WorldPieceController worldPiece = coll.GetComponent<WorldPieceController>();
				if (worldPiece)
				{
					worldPiece.miniMapPiece.SetActive(true);
				}
			}
		}
	}

	public void TakeDamage()
	{
		Instantiate(deathParticleSystem, this.transform.position, Quaternion.identity);
		heartContainers[currentHealth - 1].SetActive(false);
		currentHealth--;
		canBeDamaged = false;
		ResetCanBeDamaged();
		// Debug.Break();
	}

	public void ResetCanBeDamaged()
	{
		StartCoroutine(ResetCanBeDamaged_());
	}
	private IEnumerator ResetCanBeDamaged_()
	{
		yield return new WaitForSeconds(0.5f);
		canBeDamaged = true;
	}

	public void ResetCanBeStunned()
	{
		StartCoroutine(ResetCanBeStunned_());
	}
	private IEnumerator ResetCanBeStunned_()
	{
		yield return new WaitForSeconds(0.3f);
		canBeStunned = true;
	}
}