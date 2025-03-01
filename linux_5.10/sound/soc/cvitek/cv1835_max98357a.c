// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Machine driver for max98357a on CVITEK CV1835
 *
 * Copyright ---
 *
 * Author: ---
 *
 */

#include <linux/module.h>
#include <linux/device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <linux/io.h>
#include <linux/proc_fs.h>

// // u16 tdm_slot_no = 2;

struct card_private {
	int tmp;	//save sth.
	// struct snd_soc_jack headset;
	// struct list_head hdmi_pcm_list;
	// struct snd_soc_jack hdmi[3];
};

enum {
	DPCM_AUDIO_SPKR = 0,
};

static const struct snd_soc_dapm_widget cv1835_max98357a_dapm_widgets[] = {
	SND_SOC_DAPM_SPK("SPKR", NULL),		// diff "Speaker" in max98357a
};

static const struct snd_soc_dapm_route cv1835_max98357a_dapm_routes[] = {
	{"SPKR", NULL, "Speaker"},
};

static int cv1835_max98357a_hw_params(struct snd_pcm_substream *substream,
				     struct snd_pcm_hw_params *params)
{
	return 0;
}

static int cv1835_max98357a_asoc_init(struct snd_soc_pcm_runtime *rtd)
{
	// dual with private data


	// struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	// struct snd_soc_dai *codec_dai = rtd->codec_dai;
	// u32 ret;

	// /* Only need to set slot number while mode is TDM/PDM, otherwise default slot number is 2 */
	// if (tdm_slot_no != 2) {
	// 	ret = snd_soc_dai_set_tdm_slot(codec_dai, 0x0F, 0x0F, tdm_slot_no, 32);

	// 	if (ret < 0)
	// 		return ret;

	// 	ret = snd_soc_dai_set_tdm_slot(cpu_dai, 0x0F, 0x0F, tdm_slot_no, 32);

	// 	if (ret < 0)
	// 		return ret;
	// }

	return 0;
}

static struct snd_soc_ops cv1835_max98357a_ops = {
	.hw_params = cv1835_max98357a_hw_params,
};

SND_SOC_DAILINK_DEFS(dailink_max98357a,
	DAILINK_COMP_ARRAY(COMP_CPU("4120000.i2s")),
	DAILINK_COMP_ARRAY(COMP_CODEC("max98357a", "HiFi")),
	DAILINK_COMP_ARRAY(COMP_PLATFORM("4120000.i2s")));

static struct snd_soc_dai_link cv1835_max98357a_dai[] = {
	[DPCM_AUDIO_SPKR] = {
		.name = "max98357a",
		.stream_name = "max98357a i2s",	//"cv1835-max98357a",
		.init = cv1835_max98357a_asoc_init,
		.ops = &cv1835_max98357a_ops,
		.dai_fmt = SND_SOC_DAIFMT_I2S
		| SND_SOC_DAIFMT_IB_NF
		| SND_SOC_DAIFMT_CBS_CFS,
		SND_SOC_DAILINK_REG(dailink_max98357a),
	},
};


static struct snd_soc_card cv1835_max98357a = {
	.name = "cv1835 max98357a",	// card name
	.owner = THIS_MODULE,
	.dai_link = cv1835_max98357a_dai,
	.num_links = ARRAY_SIZE(cv1835_max98357a_dai),

	// control may don't have
	// .controls = skylake_controls,
	// .num_controls = ARRAY_SIZE(skylake_controls),

	.dapm_widgets		= cv1835_max98357a_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(cv1835_max98357a_dapm_widgets),

	.dapm_routes		= cv1835_max98357a_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(cv1835_max98357a_dapm_routes),

	// .fully_routed = true,
	// .late_probe = skylake_card_late_probe,
};

static const struct of_device_id cvi_audio_match_ids[] = {
	{
		.compatible = "cvitek,cv1835-max98357a",
		//.data = (void *) &cv1835_max98357a_dai,
	},
	{},
};
MODULE_DEVICE_TABLE(of, cvi_audio_match_ids);


static int cv1835_max98357a_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct snd_soc_card *card = &cv1835_max98357a;

	struct card_private *ctx;		// create private data for card
	int ret;

	// struct device_node *np = pdev->dev.of_node, *dai;

	dev_info(&pdev->dev, "%s, dev name=%s\n", __func__, dev_name(&pdev->dev));

	ctx = devm_kzalloc(&pdev->dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	card->dev = &pdev->dev;
	snd_soc_card_set_drvdata(card, ctx);	//save card info to snd_card

	ret = devm_snd_soc_register_card(&pdev->dev, card);
	if (ret) {
		dev_err(&pdev->dev, "snd_soc_register_card failed (%d)\n",
			ret);
		return ret;
	}

	return 0;
}

static struct platform_driver cv1835_max98357a_driver = {
	.driver = {
		.name = "cv1835-max98357a",
		.pm = &snd_soc_pm_ops,
		.of_match_table = cvi_audio_match_ids,
	},
	.probe = cv1835_max98357a_probe,
};

module_platform_driver(cv1835_max98357a_driver);

MODULE_AUTHOR("EthanChen");
MODULE_DESCRIPTION("ALSA SoC cv1835 max98357a driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cv1835-max98357a");
