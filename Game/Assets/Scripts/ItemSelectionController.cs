using UnityEngine;
using System.Collections;

public class ItemSelectionController : MonoBehaviour 
{

	public static ItemSelectionController instance;

	public bool inItemSelection;
	public GameObject selectionObjects;

	public GameObject selectorBox;

	public bool buttonReleased = true;
	public bool canMove = true;

	private float gridSize = 3;

	void Start () 
	{
		instance = this;
	}
	
	void Update () 
	{
		if (inItemSelection)
		{
			if ((Input.GetAxis("DPadVertical") > 0 || Input.GetAxis("Vertical") > 0) 
			    && buttonReleased && canMove)
			{
				canMove = false;
				StartCoroutine(ResetCanMove());
				buttonReleased = false;
				Vector3 newPos = new Vector3(selectorBox.transform.position.x, 
				                             selectorBox.transform.position.y + gridSize, 
				                             selectorBox.transform.position.z);
				selectorBox.transform.position = newPos;
			}
			if ((Input.GetAxis("DPadVertical") < 0  || Input.GetAxis("Vertical") < 0)
			    && buttonReleased && canMove)
			{
				canMove = false;
				StartCoroutine(ResetCanMove());
				buttonReleased = false;
				Vector3 newPos = new Vector3(selectorBox.transform.position.x, 
				                             selectorBox.transform.position.y - gridSize, 
				                             selectorBox.transform.position.z);
				selectorBox.transform.position = newPos;
			}
			if ((Input.GetAxis("DPadHorizontal") > 0 || Input.GetAxis("Horizontal") > 0) 
			    && buttonReleased && canMove)
			{
				canMove = false;
				StartCoroutine(ResetCanMove());
				buttonReleased = false;
				Vector3 newPos = new Vector3(selectorBox.transform.position.x + gridSize, 
				                             selectorBox.transform.position.y, 
				                             selectorBox.transform.position.z);
				selectorBox.transform.position = newPos;
			}
			if ((Input.GetAxis("DPadHorizontal") < 0 || Input.GetAxis("Horizontal") < 0)
			    && buttonReleased && canMove)
			{
				canMove = false;
				StartCoroutine(ResetCanMove());
				buttonReleased = false;
				Vector3 newPos = new Vector3(selectorBox.transform.position.x - gridSize, 
				                             selectorBox.transform.position.y, 
				                             selectorBox.transform.position.z);
				selectorBox.transform.position = newPos;
			}

			if ((Input.GetAxis("DPadVertical") == 0) && (Input.GetAxis("DPadHorizontal") == 0))
			{
				buttonReleased = true;
			}

			if (Input.GetButtonDown("ActionNorth"))
			{
				foreach(Item item in PlayerController.instance.inventory)
				{
					if (Vector3.Distance(selectorBox.transform.position, item.transform.position) < 0.5f)
					{
						PlayerController.instance.actionNorth.Equip(item);
						break;
					}
					
				}
			}
			if (Input.GetButtonDown("ActionEast"))
			{
				foreach(Item item in PlayerController.instance.inventory)
				{
					if (Vector3.Distance(selectorBox.transform.position, item.transform.position) < 0.5f)
					{
						PlayerController.instance.actionEast.Equip(item);
						break;
					}
					
				}
			}
			if (Input.GetButtonDown("ActionSouth"))
			{
				foreach(Item item in PlayerController.instance.inventory)
				{
					if (Vector3.Distance(selectorBox.transform.position, item.transform.position) < 0.5f)
					{
						PlayerController.instance.actionSouth.Equip(item);
						break;
					}
					
				}
			}
			if (Input.GetButtonDown("ActionWest"))
			{
				foreach(Item item in PlayerController.instance.inventory)
				{
					if (Vector3.Distance(selectorBox.transform.position, item.transform.position) < 0.5f)
					{
						PlayerController.instance.actionWest.Equip(item);
						break;
					}
					
				}
			}

		}
	}

	public IEnumerator ResetCanMove()
	{
		yield return new WaitForSeconds(0.2f);
		canMove = true;
	}

	public void OpenItemSelection()
	{
		selectionObjects.SetActive(true);

		Vector3 topLeft = new Vector3(this.transform.position.x - (gridSize * 4), 
		                              this.transform.position.y - (gridSize), 
		                              this.transform.position.z);
		topLeft += new Vector3(0.1f, 2f, 0);

		for (int index = 0;
		     index < PlayerController.instance.inventory.Count;
		     index++)
		{
			if (PlayerController.instance.inventory[index])
			{
				Item item = PlayerController.instance.inventory[index];
				item.stackText.text = "" + item.stackCount;
				item.gameObject.SetActive(true);
				item.transform.position = topLeft + new Vector3(index * gridSize, 1, 1);
				item.transform.rotation = Quaternion.Euler(0, 0, 0);
			}
		}
	}

	public void CloseItemSelection()
	{
		selectionObjects.SetActive(false);

		foreach (Item item in PlayerController.instance.inventory)
		{
			if (item != null)
			{
				item.gameObject.SetActive(false);
			}
		}
	}
}
