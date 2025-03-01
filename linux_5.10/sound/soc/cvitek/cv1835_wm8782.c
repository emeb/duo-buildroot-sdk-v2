// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Machine driver for wm8782 on CVITEK CV1835
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

struct card_private {
	int tmp;
};

enum {
	DPCM_AUDIO_LIN = 0,
};

static const struct snd_soc_dapm_widget cv1835_wm8782_dapm_widgets[] = {
	SND_SOC_DAPM_LINE("Line", NULL),
};

static const struct snd_soc_dapm_route cv1835_wm8782_dapm_routes[] = {
	{"Line", NULL, "Capture"},
};

static int cv1835_wm8782_hw_params(struct snd_pcm_substream *substream,
				     struct snd_pcm_hw_params *params)
{
	return 0;
}

static int cv1835_wm8782_asoc_init(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}

static struct snd_soc_ops cv1835_wm8782_ops = {
	.hw_params = cv1835_wm8782_hw_params,
};

SND_SOC_DAILINK_DEFS(dailink_wm8782,
	DAILINK_COMP_ARRAY(COMP_CPU("4110000.i2s")),
	DAILINK_COMP_ARRAY(COMP_CODEC("wm8782", "wm8782")),
	DAILINK_COMP_ARRAY(COMP_PLATFORM("4110000.i2s")));

static struct snd_soc_dai_link cv1835_wm8782_dai[] = {
	[DPCM_AUDIO_LIN] = {
		.name = "wm8782",
		.stream_name = "wm8782 i2s",
		.init = cv1835_wm8782_asoc_init,
		.ops = &cv1835_wm8782_ops,
		.dai_fmt = SND_SOC_DAIFMT_I2S
		| SND_SOC_DAIFMT_IB_NF				// invert BCLK, normal FCLK
		| SND_SOC_DAIFMT_CBS_CFS,			// CPU is master on BCLK and FCLK
		SND_SOC_DAILINK_REG(dailink_wm8782),
	},
};


static struct snd_soc_card cv1835_wm8782 = {
	.name = "cv1835 wm8782",
	.owner = THIS_MODULE,
	.dai_link = cv1835_wm8782_dai,
	.num_links = ARRAY_SIZE(cv1835_wm8782_dai),

	.dapm_widgets		= cv1835_wm8782_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(cv1835_wm8782_dapm_widgets),

	.dapm_routes		= cv1835_wm8782_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(cv1835_wm8782_dapm_routes),
};

static const struct of_device_id cvi_audio_match_ids[] = {
	{
		.compatible = "cvitek,cv1835-wm8782",
	},
	{},
};
MODULE_DEVICE_TABLE(of, cvi_audio_match_ids);


static int cv1835_wm8782_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card = &cv1835_wm8782;

	struct card_private *ctx;		// create private data for card
	int ret;

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

static struct platform_driver cv1835_wm8782_driver = {
	.driver = {
		.name = "cv1835-wm8782",
		.pm = &snd_soc_pm_ops,
		.of_match_table = cvi_audio_match_ids,
	},
	.probe = cv1835_wm8782_probe,
};

module_platform_driver(cv1835_wm8782_driver);

MODULE_AUTHOR("EthanChen");
MODULE_DESCRIPTION("ALSA SoC cv1835 wm8782 driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cv1835-wm8782");
