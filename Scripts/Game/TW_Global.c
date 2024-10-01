class TW<Class T>
{
	static T Find(IEntity entity)
	{
		if(!entity) return null;
		return T.Cast(entity.FindComponent(T));
	}
	
	static T FindOnComp(ScriptComponent comp)
	{
		if(!comp) return null;
		return T.Cast(comp.FindComponent(T));
	}
};

class TW_Global
{
	static int GetPlayerId()
	{
		IEntity playerEntity = SCR_PlayerController.GetLocalControlledEntity();
		if(!playerEntity) return 0;
		
		return GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(playerEntity);
	}
	
	static bool IsInRuntime()
	{
		if(!GetGame() || SCR_Global.IsEditMode())
			return false;
		
		return true;
	}
	
	static bool IsServer(IEntity owner)
	{
		RplComponent rpl = TW<RplComponent>.Find(owner);
		return rpl && rpl.IsMaster() && rpl.Role() == RplRole.Authority;
	}
	
	static RplComponent GetEntityRplComponentByRplId(RplId entityRplId)
	{
		Managed anything = Replication.FindItem(entityRplId);
		if(!anything)
			return null;
		
		return RplComponent.Cast(anything);
	}
	
	static IEntity GetEntityByRplId(RplId entityRplId)
	{
		RplComponent rpl = GetEntityRplComponentByRplId(entityRplId);
		return rpl.GetEntity();
	}
	
	static RplId GetEntityRplId(IEntity entity)
	{
		RplComponent rpl = TW<RplComponent>.Find(entity);
		return rpl.Id();
	}
};