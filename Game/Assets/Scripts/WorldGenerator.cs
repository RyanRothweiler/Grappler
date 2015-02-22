using UnityEngine;
using System.Collections;

public class WorldGenerator : MonoBehaviour
{
	public static WorldGenerator instance;

	void Start () 
	{
		instance = this;	
	}
	
	void Update () 
	{
	
	}

	public void GenerateWorld()
	{
		WorldPieceController[] pieceLibrary = Resources.FindObjectsOfTypeAll(typeof(WorldPieceController)) as WorldPieceController[];

		foreach(WorldPieceController piece in pieceLibrary)
		{
			if (piece.active)
			{
				DestoryImmediately(piece);
			}
		}
	}
}
