/*
 * Copyright © 2008 Intel Corporation
 *             2014 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#include <drm/drmP.h>
#include "i915_drv.h"
#include "intel_drv.h"
#include <drm/drm_atomic_helper.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_edid.h>

static bool intel_dp_mst_compute_config(struct intel_encoder *encoder,
					struct intel_crtc_state *pipe_config,
					struct drm_connector_state *conn_state)
{
	struct intel_dp_mst_encoder *intel_mst = enc_to_mst(&encoder->base);
	struct intel_digital_port *intel_dig_port = intel_mst->primary;
	struct intel_dp *intel_dp = &intel_dig_port->dp;
	struct intel_connector *connector =
		to_intel_connector(conn_state->connector);
	struct drm_atomic_state *state = pipe_config->base.state;
	int bpp;
	int lane_count, slots;
	const struct drm_display_mode *adjusted_mode = &pipe_config->base.adjusted_mode;
	int mst_pbn;
	bool reduce_m_n = drm_dp_has_quirk(&intel_dp->desc,
					   DP_DPCD_QUIRK_LIMITED_M_N);

	pipe_config->has_pch_encoder = false;
	bpp = 24;
	if (intel_dp->compliance.test_data.bpc) {
		bpp = intel_dp->compliance.test_data.bpc * 3;
		DRM_DEBUG_KMS("Setting pipe bpp to %d\n",
			      bpp);
	}
	/*
	 * for MST we always configure max link bw - the spec doesn't
	 * seem to suggest we should do otherwise.
	 */
	lane_count = drm_dp_max_lane_count(intel_dp->dpcd);
	pipe_config->lane_count = lane_count;

	pipe_config->pipe_bpp = bpp;

	pipe_config->port_clock = intel_dp_max_link_rate(intel_dp);

	if (drm_dp_mst_port_has_audio(&intel_dp->mst_mgr, connector->port))
		pipe_config->has_audio = true;

	mst_pbn = drm_dp_calc_pbn_mode(adjusted_mode->crtc_clock, bpp);
	pipe_config->pbn = mst_pbn;

	slots = drm_dp_atomic_find_vcpi_slots(state, &intel_dp->mst_mgr,
					      connector->port, mst_pbn);
	if (slots < 0) {
		DRM_DEBUG_KMS("failed finding vcpi slots:%d\n", slots);
		return false;
	}

	intel_link_compute_m_n(bpp, lane_count,
			       adjusted_mode->crtc_clock,
			       pipe_config->port_clock,
			       &pipe_config->dp_m_n,
			       reduce_m_n);

	pipe_config->dp_m_n.tu = slots;

	return true;
}

static int intel_dp_mst_atomic_check(struct drm_connector *connector,
		struct drm_connector_state *new_conn_state)
{
	struct drm_atomic_state *state = new_conn_state->state;
	struct drm_connector_state *old_conn_state;
	struct drm_crtc *old_crtc;
	struct drm_crtc_state *crtc_state;
	int slots, ret = 0;

	old_conn_state = drm_atomic_get_old_connector_state(state, connector);
	old_crtc = old_conn_state->crtc;
	if (!old_crtc)
		return ret;

	crtc_state = drm_atomic_get_new_crtc_state(state, old_crtc);
	slots = to_intel_crtc_state(crtc_state)->dp_m_n.tu;
	if (drm_atomic_crtc_needs_modeset(crtc_state) && slots > 0) {
		struct drm_dp_mst_topology_mgr *mgr;
		struct drm_encoder *old_encoder;

		old_encoder = old_conn_state->best_encoder;
		mgr = &enc_to_mst(old_encoder)->primary->dp.mst_mgr;

		ret = drm_dp_atomic_release_vcpi_slots(state, mgr, slots);
		if (ret)
			DRM_DEBUG_KMS("failed releasing %d vcpi slots:%d\n", slots, ret);
		else
			to_intel_crtc_state(crtc_state)->dp_m_n.tu = 0;
	}
	return ret;
}

static void intel_mst_disable_dp(struct intel_encoder *encoder,
				 struct intel_crtc_state *old_crtc_state,
				 struct drm_connector_state *old_conn_state)
{
	struct intel_dp_mst_encoder *intel_mst = enc_to_mst(&encoder->base);
	struct intel_digital_port *intel_dig_port = intel_mst->primary;
	struct intel_dp *intel_dp = &intel_dig_port->dp;
	struct intel_connector *connector =
		to_intel_connector(old_conn_state->connector);
	int ret;

	DRM_DEBUG_KMS("%d\n", intel_dp->active_mst_links);

	drm_dp_mst_reset_vcpi_slots(&intel_dp->mst_mgr, connector->port);

	ret = drm_dp_update_payload_part1(&intel_dp->mst_mgr);
	if (ret) {
		DRM_ERROR("failed to update payload %d\n", ret);
	}
	if (old_crtc_state->has_audio)
		intel_audio_codec_disable(encoder);
}

static void intel_mst_post_disable_dp(struct intel_encoder *encoder,
				      struct intel_crtc_state *old_crtc_state,
				      struct drm_connector_state *old_conn_state)
{
	struct intel_dp_mst_encoder *intel_mst = enc_to_mst(&encoder->base);
	struct intel_digital_port *intel_dig_port = intel_mst->primary;
	struct intel_dp *intel_dp = &intel_dig_port->dp;
	struct intel_connector *connector =
		to_intel_connector(old_conn_state->connector);

	DRM_DEBUG_KMS("%d\n", intel_dp->active_mst_links);

	/* this can fail */
	drm_dp_check_act_status(&intel_dp->mst_mgr);
	/* and this can also fail */
	drm_dp_update_payload_part2(&intel_dp->mst_mgr);

	drm_dp_mst_deallocate_vcpi(&intel_dp->mst_mgr, connector->port);

	intel_dp->active_mst_links--;

	intel_mst->connector = NULL;
	if (intel_dp->active_mst_links == 0) {
		intel_dig_port->base.post_disable(&intel_dig_port->base,
						  NULL, NULL);

		intel_dp_sink_dpms(intel_dp, DRM_MODE_DPMS_OFF);
	}
}

static void intel_mst_pre_enable_dp(struct intel_encoder *encoder,
				    struct intel_crtc_state *pipe_config,
				    struct drm_connector_state *conn_state)
{
	struct intel_dp_mst_encoder *intel_mst = enc_to_mst(&encoder->base);
	struct intel_digital_port *intel_dig_port = intel_mst->primary;
	struct intel_dp *intel_dp = &intel_dig_port->dp;
	struct drm_i915_private *dev_priv = to_i915(encoder->base.dev);
	enum port port = intel_dig_port->port;
	struct intel_connector *connector =
		to_intel_connector(conn_state->connector);
	int ret;
	uint32_t temp;

	/* MST encoders are bound to a crtc, not to a connector,
	 * force the mapping here for get_hw_state.
	 */
	connector->encoder = encoder;
	intel_mst->connector = connector;

	DRM_DEBUG_KMS("%d\n", intel_dp->active_mst_links);

	if (intel_dp->active_mst_links == 0)
		intel_dig_port->base.pre_enable(&intel_dig_port->base,
						pipe_config, NULL);

	ret = drm_dp_mst_allocate_vcpi(&intel_dp->mst_mgr,
				       connector->port,
				       pipe_config->pbn,
				       pipe_config->dp_m_n.tu);
	if (ret == false) {
		DRM_ERROR("failed to allocate vcpi\n");
		return;
	}


	intel_dp->active_mst_links++;
	temp = I915_READ(DP_TP_STATUS(port));
	I915_WRITE(DP_TP_STATUS(port), temp);

	ret = drm_dp_update_payload_part1(&intel_dp->mst_mgr);
}

static void intel_mst_enable_dp(struct intel_encoder *encoder,
				struct intel_crtc_state *pipe_config,
				struct drm_connector_state *conn_state)
{
	struct intel_dp_mst_encoder *intel_mst = enc_to_mst(&encoder->base);
	struct intel_digital_port *intel_dig_port = intel_mst->primary;
	struct intel_dp *intel_dp = &intel_dig_port->dp;
	struct drm_i915_private *dev_priv = to_i915(encoder->base.dev);
	enum port port = intel_dig_port->port;
	int ret;

	DRM_DEBUG_KMS("%d\n", intel_dp->active_mst_links);

	if (intel_wait_for_register(dev_priv,
				    DP_TP_STATUS(port),
				    DP_TP_STATUS_ACT_SENT,
				    DP_TP_STATUS_ACT_SENT,
				    1))
		DRM_ERROR("Timed out waiting for ACT sent\n");

	ret = drm_dp_check_act_status(&intel_dp->mst_mgr);

	ret = drm_dp_update_payload_part2(&intel_dp->mst_mgr);
	if (pipe_config->has_audio)
		intel_audio_codec_enable(encoder, pipe_config, conn_state);
}

static bool intel_dp_mst_enc_get_hw_state(struct intel_encoder *encoder,
				      enum pipe *pipe)
{
	struct intel_dp_mst_encoder *intel_mst = enc_to_mst(&encoder->base);
	*pipe = intel_mst->pipe;
	if (intel_mst->connector)
		return true;
	return false;
}

static void intel_dp_mst_enc_get_config(struct intel_encoder *encoder,
					struct intel_crtc_state *pipe_config)
{
	struct intel_dp_mst_encoder *intel_mst = enc_to_mst(&encoder->base);
	struct intel_digital_port *intel_dig_port = intel_mst->primary;
	struct intel_crtc *crtc = to_intel_crtc(pipe_config->base.crtc);
	struct drm_i915_private *dev_priv = to_i915(encoder->base.dev);
	enum transcoder cpu_transcoder = pipe_config->cpu_transcoder;
	u32 temp, flags = 0;

	pipe_config->has_audio =
		intel_ddi_is_audio_enabled(dev_priv, crtc);

	temp = I915_READ(TRANS_DDI_FUNC_CTL(cpu_transcoder));
	if (temp & TRANS_DDI_PHSYNC)
		flags |= DRM_MODE_FLAG_PHSYNC;
	else
		flags |= DRM_MODE_FLAG_NHSYNC;
	if (temp & TRANS_DDI_PVSYNC)
		flags |= DRM_MODE_FLAG_PVSYNC;
	else
		flags |= DRM_MODE_FLAG_NVSYNC;

	switch (temp & TRANS_DDI_BPC_MASK) {
	case TRANS_DDI_BPC_6:
		pipe_config->pipe_bpp = 18;
		break;
	case TRANS_DDI_BPC_8:
		pipe_config->pipe_bpp = 24;
		break;
	case TRANS_DDI_BPC_10:
		pipe_config->pipe_bpp = 30;
		break;
	case TRANS_DDI_BPC_12:
		pipe_config->pipe_bpp = 36;
		break;
	default:
		break;
	}
	pipe_config->base.adjusted_mode.flags |= flags;

	pipe_config->lane_count =
		((temp & DDI_PORT_WIDTH_MASK) >> DDI_PORT_WIDTH_SHIFT) + 1;

	intel_dp_get_m_n(crtc, pipe_config);

	intel_ddi_clock_get(&intel_dig_port->base, pipe_config);
}

static int intel_dp_mst_get_ddc_modes(struct drm_connector *connector)
{
	struct intel_connector *intel_connector = to_intel_connector(connector);
	struct intel_dp *intel_dp = intel_connector->mst_port;
	struct edid *edid;
	int ret;

	if (!intel_dp) {
		return intel_connector_update_modes(connector, NULL);
	}

	edid = drm_dp_mst_get_edid(connector, &intel_dp->mst_mgr, intel_connector->port);
	ret = intel_connector_update_modes(connector, edid);
	kfree(edid);

	return ret;
}

static enum drm_connector_status
intel_dp_mst_detect(struct drm_connector *connector, bool force)
{
	struct intel_connector *intel_connector = to_intel_connector(connector);
	struct intel_dp *intel_dp = intel_connector->mst_port;

	if (!intel_dp)
		return connector_status_disconnected;
	return drm_dp_mst_detect_port(connector, &intel_dp->mst_mgr, intel_connector->port);
}

static void
intel_dp_mst_connector_destroy(struct drm_connector *connector)
{
	struct intel_connector *intel_connector = to_intel_connector(connector);

	if (!IS_ERR_OR_NULL(intel_connector->edid))
		kfree(intel_connector->edid);

	drm_connector_cleanup(connector);
	kfree(connector);
}

static const struct drm_connector_funcs intel_dp_mst_connector_funcs = {
	.dpms = drm_atomic_helper_connector_dpms,
	.detect = intel_dp_mst_detect,
	.fill_modes = drm_helper_probe_single_connector_modes,
	.late_register = intel_connector_register,
	.early_unregister = intel_connector_unregister,
	.destroy = intel_dp_mst_connector_destroy,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
};

static int intel_dp_mst_get_modes(struct drm_connector *connector)
{
	return intel_dp_mst_get_ddc_modes(connector);
}

static enum drm_mode_status
intel_dp_mst_mode_valid(struct drm_connector *connector,
			struct drm_display_mode *mode)
{
	struct intel_connector *intel_connector = to_intel_connector(connector);
	struct intel_dp *intel_dp = intel_connector->mst_port;
	int max_dotclk = to_i915(connector->dev)->max_dotclk_freq;
	int bpp = 24; /* MST uses fixed bpp */
	int max_rate, mode_rate, max_lanes, max_link_clock;

	max_link_clock = intel_dp_max_link_rate(intel_dp);
	max_lanes = intel_dp_max_lane_count(intel_dp);

	max_rate = intel_dp_max_data_rate(max_link_clock, max_lanes);
	mode_rate = intel_dp_link_required(mode->clock, bpp);

	/* TODO - validate mode against available PBN for link */
	if (mode->clock < 10000)
		return MODE_CLOCK_LOW;

	if (mode->flags & DRM_MODE_FLAG_DBLCLK)
		return MODE_H_ILLEGAL;

	if (mode_rate > max_rate || mode->clock > max_dotclk)
		return MODE_CLOCK_HIGH;

	return MODE_OK;
}

static struct drm_encoder *intel_mst_atomic_best_encoder(struct drm_connector *connector,
							 struct drm_connector_state *state)
{
	struct intel_connector *intel_connector = to_intel_connector(connector);
	struct intel_dp *intel_dp = intel_connector->mst_port;
	struct intel_crtc *crtc = to_intel_crtc(state->crtc);

	if (!intel_dp)
		return NULL;
	return &intel_dp->mst_encoders[crtc->pipe]->base.base;
}

static struct drm_encoder *intel_mst_best_encoder(struct drm_connector *connector)
{
	struct intel_connector *intel_connector = to_intel_connector(connector);
	struct intel_dp *intel_dp = intel_connector->mst_port;
	if (!intel_dp)
		return NULL;
	return &intel_dp->mst_encoders[0]->base.base;
}

static const struct drm_connector_helper_funcs intel_dp_mst_connector_helper_funcs = {
	.get_modes = intel_dp_mst_get_modes,
	.mode_valid = intel_dp_mst_mode_valid,
	.atomic_best_encoder = intel_mst_atomic_best_encoder,
	.best_encoder = intel_mst_best_encoder,
	.atomic_check = intel_dp_mst_atomic_check,
};

static void intel_dp_mst_encoder_destroy(struct drm_encoder *encoder)
{
	struct intel_dp_mst_encoder *intel_mst = enc_to_mst(encoder);

	drm_encoder_cleanup(encoder);
	kfree(intel_mst);
}

static const struct drm_encoder_funcs intel_dp_mst_enc_funcs = {
	.destroy = intel_dp_mst_encoder_destroy,
};

static bool intel_dp_mst_get_hw_state(struct intel_connector *connector)
{
	if (connector->encoder && connector->base.state->crtc) {
		enum pipe pipe;
		if (!connector->encoder->get_hw_state(connector->encoder, &pipe))
			return false;
		return true;
	}
	return false;
}

static struct drm_connector *intel_dp_add_mst_connector(struct drm_dp_mst_topology_mgr *mgr, struct drm_dp_mst_port *port, const char *pathprop)
{
	struct intel_dp *intel_dp = container_of(mgr, struct intel_dp, mst_mgr);
	struct intel_digital_port *intel_dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = intel_dig_port->base.base.dev;
	struct intel_connector *intel_connector;
	struct drm_connector *connector;
	int i;

	intel_connector = intel_connector_alloc();
	if (!intel_connector)
		return NULL;

	connector = &intel_connector->base;
	drm_connector_init(dev, connector, &intel_dp_mst_connector_funcs, DRM_MODE_CONNECTOR_DisplayPort);
	drm_connector_helper_add(connector, &intel_dp_mst_connector_helper_funcs);

	intel_connector->get_hw_state = intel_dp_mst_get_hw_state;
	intel_connector->mst_port = intel_dp;
	intel_connector->port = port;

	for (i = PIPE_A; i <= PIPE_C; i++) {
		drm_mode_connector_attach_encoder(&intel_connector->base,
						  &intel_dp->mst_encoders[i]->base.base);
	}

	drm_object_attach_property(&connector->base, dev->mode_config.path_property, 0);
	drm_object_attach_property(&connector->base, dev->mode_config.tile_property, 0);

	drm_mode_connector_set_path_property(connector, pathprop);
	return connector;
}

static void intel_dp_register_mst_connector(struct drm_connector *connector)
{
	struct drm_i915_private *dev_priv = to_i915(connector->dev);

	if (dev_priv->fbdev)
		drm_fb_helper_add_one_connector(&dev_priv->fbdev->helper,
						connector);

	drm_connector_register(connector);
}

static void intel_dp_destroy_mst_connector(struct drm_dp_mst_topology_mgr *mgr,
					   struct drm_connector *connector)
{
	struct intel_connector *intel_connector = to_intel_connector(connector);
	struct drm_i915_private *dev_priv = to_i915(connector->dev);

	drm_connector_unregister(connector);

	if (dev_priv->fbdev)
		drm_fb_helper_remove_one_connector(&dev_priv->fbdev->helper,
						   connector);
	/* prevent race with the check in ->detect */
	drm_modeset_lock(&connector->dev->mode_config.connection_mutex, NULL);
	intel_connector->mst_port = NULL;
	drm_modeset_unlock(&connector->dev->mode_config.connection_mutex);

	drm_connector_unreference(connector);
	DRM_DEBUG_KMS("\n");
}

static void intel_dp_mst_hotplug(struct drm_dp_mst_topology_mgr *mgr)
{
	struct intel_dp *intel_dp = container_of(mgr, struct intel_dp, mst_mgr);
	struct intel_digital_port *intel_dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = intel_dig_port->base.base.dev;

	drm_kms_helper_hotplug_event(dev);
}

static const struct drm_dp_mst_topology_cbs mst_cbs = {
	.add_connector = intel_dp_add_mst_connector,
	.register_connector = intel_dp_register_mst_connector,
	.destroy_connector = intel_dp_destroy_mst_connector,
	.hotplug = intel_dp_mst_hotplug,
};

static struct intel_dp_mst_encoder *
intel_dp_create_fake_mst_encoder(struct intel_digital_port *intel_dig_port, enum pipe pipe)
{
	struct intel_dp_mst_encoder *intel_mst;
	struct intel_encoder *intel_encoder;
	struct drm_device *dev = intel_dig_port->base.base.dev;

	intel_mst = kzalloc(sizeof(*intel_mst), GFP_KERNEL);

	if (!intel_mst)
		return NULL;

	intel_mst->pipe = pipe;
	intel_encoder = &intel_mst->base;
	intel_mst->primary = intel_dig_port;

	drm_encoder_init(dev, &intel_encoder->base, &intel_dp_mst_enc_funcs,
			 DRM_MODE_ENCODER_DPMST, "DP-MST %c", pipe_name(pipe));

	intel_encoder->type = INTEL_OUTPUT_DP_MST;
	intel_encoder->power_domain = intel_dig_port->base.power_domain;
	intel_encoder->port = intel_dig_port->port;
	intel_encoder->crtc_mask = 0x7;
	intel_encoder->cloneable = 0;

	intel_encoder->compute_config = intel_dp_mst_compute_config;
	intel_encoder->disable = intel_mst_disable_dp;
	intel_encoder->post_disable = intel_mst_post_disable_dp;
	intel_encoder->pre_enable = intel_mst_pre_enable_dp;
	intel_encoder->enable = intel_mst_enable_dp;
	intel_encoder->get_hw_state = intel_dp_mst_enc_get_hw_state;
	intel_encoder->get_config = intel_dp_mst_enc_get_config;

	return intel_mst;

}

static bool
intel_dp_create_fake_mst_encoders(struct intel_digital_port *intel_dig_port)
{
	int i;
	struct intel_dp *intel_dp = &intel_dig_port->dp;

	for (i = PIPE_A; i <= PIPE_C; i++)
		intel_dp->mst_encoders[i] = intel_dp_create_fake_mst_encoder(intel_dig_port, i);
	return true;
}

int
intel_dp_mst_encoder_init(struct intel_digital_port *intel_dig_port, int conn_base_id)
{
	struct intel_dp *intel_dp = &intel_dig_port->dp;
	struct drm_device *dev = intel_dig_port->base.base.dev;
	int ret;

	intel_dp->can_mst = true;
	intel_dp->mst_mgr.cbs = &mst_cbs;

	/* create encoders */
	intel_dp_create_fake_mst_encoders(intel_dig_port);
	ret = drm_dp_mst_topology_mgr_init(&intel_dp->mst_mgr, dev,
					   &intel_dp->aux, 16, 3, conn_base_id);
	if (ret) {
		intel_dp->can_mst = false;
		return ret;
	}
	return 0;
}

void
intel_dp_mst_encoder_cleanup(struct intel_digital_port *intel_dig_port)
{
	struct intel_dp *intel_dp = &intel_dig_port->dp;

	if (!intel_dp->can_mst)
		return;

	drm_dp_mst_topology_mgr_destroy(&intel_dp->mst_mgr);
	/* encoders will get killed by normal cleanup */
}
