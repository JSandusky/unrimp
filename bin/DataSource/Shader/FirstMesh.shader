{
	"Format":
	{
		"Type": "ShaderAsset",
		"Version": "1"
	},
	"Shader":
	{
		"Properties":
		{
			"DiffuseMap":
			{
				"Name": "Diffuse Map",
				"Description": "Diffuse map",
				"Type": "Texture2D",
				"Default": "White"
			},
			"EmissiveMap":
			{
				"Name": "Emissive Map",
				"Description": "Emissive map",
				"Type": "Texture2D",
				"Default": "Black"
			},
			"NormalMap":
			{
				"Name": "Normal Map",
				"Description": "Normal map",
				"Type": "Texture2D",
				"Default": "White"
			},
			"SpecularMap":
			{
				"Name": "Specular Map",
				"Description": "Specular map",
				"Type": "Texture2D",
				"Default": "Black"
			}
		},
		"Technique":
		{
			"Tags":
			{
				"RenderMode": "Solid"
			},
			"Pass":
			{
				"RasterizerState":
				{
					"FillMode": "SOLID",
					"CullMode": "BACK",
					"FrontCounterClockwise": "false",
					"DepthBias": "0",
					"DepthBiasClamp": "0.0",
					"SlopeScaledDepthBias": "0.0",
					"DepthClipEnable": "true",
					"ScissorEnable": "false",
					"MultisampleEnable": "false",
					"AntialiasedLineEnable": "false"
				},
				"DepthStencilState":
				{
					"DepthEnable": "true",
					"DepthWriteMask": "ALL",
					"DepthFunc": "LESS",
					"StencilEnable": "false",
					"StencilReadMask": "255",
					"StencilWriteMask": "255",
					"FrontFace":
					{
						"StencilFailOp": "KEEP",
						"StencilDepthFailOp": "KEEP",
						"StencilPassOp": "KEEP",
						"StencilFunc": "ALWAYS"
					},
					"BackFace":
					{
						"StencilFailOp": "KEEP",
						"StencilDepthFailOp": "KEEP",
						"StencilPassOp": "KEEP",
						"StencilFunc": "ALWAYS"
					}
				},
				"BlendState":
				{
					"AlphaToCoverageEnable": "false",
					"IndependentBlendEnable": "false",
					"RenderTarget[0]":
					{
						"BlendEnable": "false",
						"SrcBlend": "ONE",
						"DestBlend": "ZERO",
						"BlendOp": "ADD",
						"SrcBlendAlpha": "ONE",
						"DestBlendAlpha": "ZERO",
						"BlendOpAlpha": "ADD",
						"RenderTargetWriteMask": "ALL"
					},
					"RenderTarget[1]":
					{
						"BlendEnable": "false",
						"SrcBlend": "ONE",
						"DestBlend": "ZERO",
						"BlendOp": "ADD",
						"SrcBlendAlpha": "ONE",
						"DestBlendAlpha": "ZERO",
						"BlendOpAlpha": "ADD",
						"RenderTargetWriteMask": "ALL"
					},
					"RenderTarget[2]":
					{
						"BlendEnable": "false",
						"SrcBlend": "ONE",
						"DestBlend": "ZERO",
						"BlendOp": "ADD",
						"SrcBlendAlpha": "ONE",
						"DestBlendAlpha": "ZERO",
						"BlendOpAlpha": "ADD",
						"RenderTargetWriteMask": "ALL"
					},
					"RenderTarget[3]":
					{
						"BlendEnable": "false",
						"SrcBlend": "ONE",
						"DestBlend": "ZERO",
						"BlendOp": "ADD",
						"SrcBlendAlpha": "ONE",
						"DestBlendAlpha": "ZERO",
						"BlendOpAlpha": "ADD",
						"RenderTargetWriteMask": "ALL"
					},
					"RenderTarget[4]":
					{
						"BlendEnable": "false",
						"SrcBlend": "ONE",
						"DestBlend": "ZERO",
						"BlendOp": "ADD",
						"SrcBlendAlpha": "ONE",
						"DestBlendAlpha": "ZERO",
						"BlendOpAlpha": "ADD",
						"RenderTargetWriteMask": "ALL"
					},
					"RenderTarget[5]":
					{
						"BlendEnable": "false",
						"SrcBlend": "ONE",
						"DestBlend": "ZERO",
						"BlendOp": "ADD",
						"SrcBlendAlpha": "ONE",
						"DestBlendAlpha": "ZERO",
						"BlendOpAlpha": "ADD",
						"RenderTargetWriteMask": "ALL"
					},
					"RenderTarget[6]":
					{
						"BlendEnable": "false",
						"SrcBlend": "ONE",
						"DestBlend": "ZERO",
						"BlendOp": "ADD",
						"SrcBlendAlpha": "ONE",
						"DestBlendAlpha": "ZERO",
						"BlendOpAlpha": "ADD",
						"RenderTargetWriteMask": "ALL"
					},
					"RenderTarget[7]":
					{
						"BlendEnable": "false",
						"SrcBlend": "ONE",
						"DestBlend": "ZERO",
						"BlendOp": "ADD",
						"SrcBlendAlpha": "ONE",
						"DestBlendAlpha": "ZERO",
						"BlendOpAlpha": "ADD",
						"RenderTargetWriteMask": "ALL"
					}
				},
				"Program":
				{
					"VertexProgram":
					{
						"SourceCode": "FirstMesh.hlsl",
						"EntryPoint": "VertexProgramMain"
					},
					"FragmentProgram":
					{
						"SourceCode": "FirstMesh.hlsl",
						"EntryPoint": "FragmentProgramMain"
					},
					"ProgramPermutation":
					{
						"LightingQuality": [ "LOW_LIGHTING_QUALITY, MEDIUM_LIGHTING_QUALITY", "HIGH_LIGHTING_QUALITY" ],
						"UseDiffuseMap": [ "DIFFUSE_MAP_ON", "DIFFUSE_MAP_OFF" ]
					}
				},
				"SamplerState":
				{
					"SamplerLinear":
					{
						"Filter": "MIN_MAG_MIP_LINEAR",
						"AddressU": "CLAMP",
						"AddressV": "CLAMP",
						"AddressW": "CLAMP",
						"MipLODBias": "0.0",
						"MaxAnisotropy": "16",
						"ComparisonFunc": "NEVER",
						"BorderColor": "0.0 0.0 0.0 0.0",
						"MinLOD": "-3.402823466e+38",
						"MaxLOD": "3.402823466e+38"
					}
				}
			}
		}
	}
}
