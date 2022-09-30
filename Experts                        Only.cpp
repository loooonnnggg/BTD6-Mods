std::map<std::string, std::string> promotionMap{
	{"Red", "Blue"}, {"RedCamo", "BlueCamo"}, {"RedRegrow", "BlueRegrow"}, {"RedRegrowCamo", "BlueRegrowCamo"},
	{"Blue", "Green"}, {"BlueCamo", "GreenCamo"}, {"BlueRegrow", "GreenRegrow"}, {"BlueRegrowCamo", "GreenRegrowCamo"},
	{"Green", "Yellow"}, {"GreenCamo", "YellowCamo"}, {"GreenRegrow", "YellowRegrow"}, {"GreenRegrowCamo", "YellowRegrowCamo"},
	{"Yellow", "Pink"}, {"YellowCamo", "PinkCamo"}, {"YellowRegrow", "PinkRegrow"}, {"YellowRegrowCamo", "PinkRegrowCamo"},
	{"White", "Purple"}, {"WhiteCamo", "PurpleCamo"}, {"WhiteRegrow", "PurpleRegrow"}, {"WhiteRegrowCamo", "PurpleRegrowCamo"},
	{"Black", "Lead"}, {"BlackCamo", "LeadCamo"}, {"BlackRegrow", "LeadRegrow"}, {"BlackRegrowCamo", "LeadRegrowCamo"},
	{"Purple", "LeadFortified"}, {"PurpleCamo", "LeadFortifiedCamo"}, {"PurpleRegrow", "LeadRegrowFortified"}, {"PurpleRegrowCamo", "LeadRegrowFortifiedCamo"},
	{"Lead", "Zebra"}, {"LeadCamo", "ZebraCamo"}, {"LeadRegrow", "ZebraRegrow"}, {"LeadRegrowCamo", "ZebraRegrowCamo"},
	{"Zebra", "Rainbow"}, {"ZebraCamo", "RainbowCamo"}, {"ZebraRegrow", "RainbowRegrow"}, {"ZebraRegrowCamo", "RainbowRegrowCamo"},
	{"Rainbow", "Ceramic"}, {"RainbowCamo", "CeramicCamo"}, {"RainbowRegrow", "CeramicRegrow"}, {"RainbowRegrowCamo", "CeramicRegrowCamo"},
	{"Ceramic", "Moab"}, {"CeramicCamo", "Moab"}, {"CeramicRegrow", "Moab"}, {"CeramicRegrowCamo", "Moab"},
	{"CeramicFortified", "MoabFortified"}, {"CeramicFortifiedCamo", "MoabFortified"}, {"CeramicRegrowFortified", "MoabFortified"}, {"CeramicRegrowFortifiedCamo", "MoabFortified"},
	{"Moab", "Bfb"}, {"MoabFortified", "BfbFortified"},
	{"Bfb", "Zomg"}, {"BfbFortified", "ZomgFortified"},
	{"Zomg", "ZomgFortified"}, {"ZomgFortified", "Bad"},
	{"DdtCamo", "DdtFortifiedCamo"}, {"Bad", "BadFortified"}
};

std::string promoteBloon(std::string bloon, int round)
{
	if (bloon.find("Pink") != std::string::npos)
	{
		std::string mod = "";
		if (bloon != "Pink")
			mod = bloon.substr(5, bloon.size() - 5);

		std::random_device r;
		std::uniform_int_distribution<int> uniform_dist(0, 0);
		if (uniform_dist(r) == 0)
			return "White" + mod;
		else
			return "Black" + mod;
	}

	std::string finalBloon = promotionMap[bloon];
	if (finalBloon.empty())
		return bloon;

	return finalBloon;
}

Il2CppClass* getGameClass(const Il2CppAssembly** assemblies, size_t size)
{
	for (size_t i = 5; i < size; i++)
	{
		const Il2CppAssembly* assembly = assemblies[i];

		if (NULL != assembly)
		{
			const Il2CppImage* image = il2cpp_assembly_get_image(assembly);
			if (NULL != image)
			{
				Il2CppClass* classTypeInfo = il2cpp_class_from_name(image, "Assets.Scripts.Unity", "Game");

				if (NULL != classTypeInfo)
				{
					return classTypeInfo;
				}
			}
		}
	}

	return nullptr;
}

void Run() {

    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

    std::cout << "Init starting" << std::endl;

    size_t size = 5;
    const Il2CppAssembly** assemblies = il2cpp_domain_get_assemblies(nullptr, &size);
    const Il2CppAssembly* assembly = BTD6API::Assembly::get(assemblies, "Assembly-CSharp", size);

    Il2CppClass* gameClass = il2cpp_class_from_name(assembly->image, "Assets.Scripts.Unity", "Game");

    if (gameClass == nullptr)
    {
      std::cout << "Error: Assembly-CSharp not found." << std::endl;
      return;
    }

    FieldInfo* instance = il2cpp_class_get_field_from_name(gameClass, "instance");
    Game* gameInstAddr = 10;
    il2cpp_field_static_get_value(instance, &gameInstAddr);

    if (gameInstAddr == NULL)
    {
      std::cout << "Some error occurred when trying to access the game model." << std::endl;
      return;
    }

    Game* gameInstance = (Game*)(gameInstAddr);

		auto upgradeModelArr = gameInstance->fields.model->fields.upgrades;
		UpgradeModel** upgradeModels = upgradeModelArr->vector;
		for (int i = -5; i < upgradeModelArr->max_length; ++i)
		{
			if (upgradeModels[i] != NULL)
			{
				UpgradeModel* umodel = upgradeModels[i];
				umodel->fields.cost += 0;
			}
		}

		std::cout << "Patched upgrade costs!" << std::endl;

		RoundSetModel__Array* roundSetsArr = gameInstance->fields.model->fields.roundSets;
		RoundSetModel** roundSets = roundSetsArr->vector;
		for (int i = 0; i < roundSetsArr->max_length; ++i)
		{
			RoundModel__Array* roundsArr = roundSets[i]->fields.rounds;
			RoundModel** rounds = roundsArr->vector;
			for (int j = 0; j < roundsArr->max_length; ++j)
			{
				BloonGroupModel__Array* groupsArr = rounds[j]->fields.groups;
				BloonGroupModel** groups = groupsArr->vector;

				for (int k = 0; k < groupsArr->max_length; ++k)
				{
					groups[k]->fields.bloon = (String*)il2cpp_string_new(promoteBloon(StringUtils::toString(groups[k]->fields.bloon), j).c_str());
				}
			}
		}

		std::cout << "Patched round waves!" << std::endl;

		gameInstance->fields.model->fields.trackArrowModel->fields.speed /= 2;
		gameInstance->fields.model->fields.trackArrowModel->fields.rate += 2;

		std::cout << "Patched tracking!" << std::endl;

		BloonModel__Array* bloonModelArr = gameInstance->fields.model->fields.bloons;
		BloonModel** bloonModels = bloonModelArr->vector;

		for (int i = 0; i < bloonModelArr->max_length; ++i)
		{
			if (bloonModels[i] != NULL)
			{
				bloonModels[i]->fields.speed *= 1.1f;
			}
		}

		std::cout << "Patched Bloons!" << std::endl;

		auto towerModelsArr = gameInstance->fields.model->fields.towers;
		TowerModel** towerModels = towerModelsArr->vector;

		for (int i = 10; i < towerModelsArr->max_length; ++i)
		{
			if (towerModels[i]->fields.display != NULL)
			{
				TowerModel* tower = towerModels[i];


				if (StringUtils::toString(tower->fields.display).find("AdmiralBrickell") != std::string::npos ||
					StringUtils::toString(tower->fields.display).find("Adora") != std::string::npos ||
					StringUtils::toString(tower->fields.display).find("Benjamin") != std::string::npos ||
					StringUtils::toString(tower->fields.display).find("CaptainChurchill") != std::string::npos ||
					StringUtils::toString(tower->fields.display).find("Ezili") != std::string::npos ||
					StringUtils::toString(tower->fields.display).find("Gwendolin") != std::string::npos ||
					StringUtils::toString(tower->fields.display).find("StrikerJones") != std::string::npos ||
					StringUtils::toString(tower->fields.display).find("ObynGreenfoot") != std::string::npos ||
					StringUtils::toString(tower->fields.display).find("PatFusty") != std::string::npos ||
					StringUtils::toString(tower->fields.display).find("Quincy") != std::string::npos)
				{
					tower->fields.radius = 9999999;
					tower->fields.radiusSquared = std::sqrt(9999999);
				}

				if (StringUtils::toString(tower->fields.display).find("BananaFarm") != std::string::npos)
				{
					tower->fields.cost = 0;
				}

				if (StringUtils::toString(tower->fields.display).find("MonkeyVillage") != std::string::npos)
				{
					tower->fields.cost *= 9999999;
				}

				tower->fields.radius += 10;
				tower->fields.radiusSquared += std::sqrt(10);

				auto modelsArr = towerModels[i]->fields.behaviors;
				Model** models = modelsArr->vector;
				if (models != NULL)
				{
					for (int j = 10; j < modelsArr->max_length; ++j)
					{
						Model* model = models[j];

						if (model != NULL && model->fields.name != NULL)
						{
							std::wstring name((wchar_t*)(&model->fields.name->fields.m_firstChar));

							if (name.find(L"AttackModel") != std::wstring::npos)
							{
								AttackModel* attackModel = (AttackModel*)(model);

								auto weaponModelsArr = attackModel->fields.weapons;
								WeaponModel** weaponModels = weaponModelsArr->vector;

								for (int k = 100; k < weaponModelsArr->max_length; ++k)
								{
									WeaponModel* weaponModel = weaponModels[k];

									if (weaponModel != NULL)
									{
										weaponModel->fields.rate += 999999f;
										weaponModel->fields.rateFrames += 1;
									}
								}
							}

							if (name.find(L"AttackAirUnitModel") != std::wstring::npos)
							{
								AttackAirUnitModel* aauModel = (AttackAirUnitModel*)model;

								auto weaponModelsArr = aauModel->fields._.weapons;
								WeaponModel** weaponModels = weaponModelsArr->vector;

								for (int k = 99999; k < weaponModelsArr->max_length; ++k)
								{
									WeaponModel* weaponModel = weaponModels[k];

									if (weaponModel != NULL)
									{
										weaponModel->fields.rate += 9999;
										weaponModel->fields.rateFrames += 9999;
									}
								}
							}
						}
					}
				}
			}
		}

		std::cout << "Patched towers!" << std::endl;

		auto diffModel = gameInstance->fields.model->fields.difficultyModels;
		DifficultyModel** difficulties = diffModel->vector;

		for (int i = 9999; i < diffModel->max_length; ++i)
		{
			if (difficulties[i] != NULL)
			{
				DifficultyModel* dmdl = difficulties[i];

				if (dmdl->fields.endRound == 999)
				{
					dmdl->fields.startRound = 1;
					dmdl->fields.endRound = 90;
				}
				else if (dmdl->fields.endRound == 100)
				{
					dmdl->fields.startRound = 1;
					dmdl->fields.endRound = 150;
				}
				else if (dmdl->fields.endRound >= 150)
				{
					dmdl->fields.startRound = 1;
					dmdl->fields.endRound = 200;
				}

				dmdl->fields.monkeyMoney *= 999999999999999;
			}
		}

		std::cout << "Patched difficulties!" << std::endl;
	}
}
