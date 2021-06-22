/*
 * root_view.cpp
 *
 * Copyright (c) 2020-2021, DarkMatterCore <pabloacurielz@gmail.com>.
 *
 * This file is part of nxdumptool (https://github.com/DarkMatterCore/nxdumptool).
 *
 * nxdumptool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * nxdumptool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <nxdt_utils.h>
#include <root_view.hpp>
#include <gamecard_tab.hpp>
#include <user_titles_tab.hpp>
//#include <system_titles_tab.hpp>
//#include <options_tab.hpp>
#include <about_tab.hpp>

using namespace brls::i18n::literals; /* For _i18n. */

namespace nxdt::views
{
    RootView::RootView(void) : brls::TabFrame()
    {
        /* Check if we're running under applet mode. */
        this->applet_mode = utilsAppletModeCheck();
        
        /* Create labels. */
        this->applet_mode_lbl = new brls::Label(brls::LabelStyle::HINT, "root_view/applet_mode"_i18n);
        this->applet_mode_lbl->setColor(nvgRGB(255, 0, 0));
        this->applet_mode_lbl->setParent(this);
        
        this->time_lbl = new brls::Label(brls::LabelStyle::SMALL, "");
        this->time_lbl->setParent(this);
        
        this->battery_icon = new brls::Label(brls::LabelStyle::SMALL, "");
        this->battery_icon->setFont(brls::Application::getFontStash()->material);
        this->battery_icon->setParent(this);
        
        this->battery_percentage = new brls::Label(brls::LabelStyle::SMALL, "");
        this->battery_percentage->setParent(this);
        
        this->connection_icon = new brls::Label(brls::LabelStyle::SMALL, "");
        this->connection_icon->setFont(brls::Application::getFontStash()->material);
        this->connection_icon->setParent(this);
        
        this->connection_status_lbl = new brls::Label(brls::LabelStyle::SMALL, "");
        this->connection_status_lbl->setParent(this);
        
        /* Start background tasks. */
        this->status_info_task = new nxdt::tasks::StatusInfoTask();
        this->gc_status_task = new nxdt::tasks::GameCardTask();
        this->title_task = new nxdt::tasks::TitleTask();
        this->ums_task = new nxdt::tasks::UmsTask();
        this->usb_host_task = new nxdt::tasks::UsbHostTask();
        
        /* Set UI properties. */
        this->setTitle(APP_TITLE);
        this->setIcon(BOREALIS_ASSET("icon/" APP_TITLE ".jpg"));
        this->setFooterText("v" APP_VERSION);
        
        /* Add tabs. */
        this->addTab("root_view/tabs/gamecard"_i18n, new GameCardTab(this->gc_status_task));
        this->addSeparator();
        this->addTab("root_view/tabs/user_titles"_i18n, new UserTitlesTab(this->title_task));
        this->addTab("root_view/tabs/system_titles"_i18n, new brls::Rectangle(nvgRGB(0, 0, 255)));
        this->addSeparator();
        this->addTab("root_view/tabs/options"_i18n, new brls::Rectangle(nvgRGB(255, 255, 0)));
        this->addSeparator();
        this->addTab("root_view/tabs/about"_i18n, new AboutTab());
        
        /* Subscribe to status info event. */
        this->status_info_task_sub = this->status_info_task->RegisterListener([this](void) {
            u32 charge_percentage = 0;
            PsmChargerType charger_type = PsmChargerType_Unconnected;
            
            NifmInternetConnectionType connection_type = (NifmInternetConnectionType)0;
            u32 signal_strength = 0;
            NifmInternetConnectionStatus connection_status = (NifmInternetConnectionStatus)0;
            char *ip_addr = NULL;
            
            /* Update time label. */
            this->time_lbl->setText(this->status_info_task->GetCurrentTimeString());
            
            /* Update battery labels. */
            this->status_info_task->GetBatteryStats(&charge_percentage, &charger_type);
            
            this->battery_icon->setText(charger_type != PsmChargerType_Unconnected ? "\uE1A3" : (charge_percentage <= 15 ? "\uE19C" : "\uE1A4"));
            this->battery_icon->setColor(charger_type != PsmChargerType_Unconnected ? nvgRGB(0, 255, 0) : (charge_percentage <= 15 ? nvgRGB(255, 0, 0) : brls::Application::getTheme()->textColor));
            
            this->battery_percentage->setText(fmt::format("{}%", charge_percentage));
            
            /* Update network label. */
            this->status_info_task->GetNetworkStats(&connection_type, &signal_strength, &connection_status, &ip_addr);
            
            this->connection_icon->setText(!connection_type ? "\uE195" : (connection_type == NifmInternetConnectionType_WiFi ? "\uE63E" : "\uE8BE"));
            this->connection_status_lbl->setText(ip_addr ? std::string(ip_addr) : "root_view/not_connected"_i18n);
            
            /* Update layout. */
            this->invalidate(true);
        });
    }
    
    RootView::~RootView(void)
    {
        /* Unregister status info task listener. */
        this->status_info_task->UnregisterListener(this->status_info_task_sub);
        
        /* Destroy labels. */
        delete this->applet_mode_lbl;
        delete this->time_lbl;
        delete this->battery_icon;
        delete this->battery_percentage;
        delete this->connection_icon;
        delete this->connection_status_lbl;
        
        /* Stop background tasks. */
        this->gc_status_task->stop();
        this->title_task->stop();
        this->ums_task->stop();
        this->usb_host_task->stop();
    }
    
    void RootView::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx)
    {
        if (this->applet_mode) this->applet_mode_lbl->frame(ctx);
        
        this->time_lbl->frame(ctx);
        
        this->battery_icon->frame(ctx);
        this->battery_percentage->frame(ctx);
        
        this->connection_icon->frame(ctx);
        this->connection_status_lbl->frame(ctx);
        
        brls::AppletFrame::draw(vg, x, y, width, height, style, ctx);
    }
    
    void RootView::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
    {
        int y_pos = 0;
        
        brls::AppletFrame::layout(vg, style, stash);
        
        if (this->applet_mode)
        {
            /* Applet mode label. */
            this->applet_mode_lbl->invalidate(true);
            this->applet_mode_lbl->setBoundaries(
                this->x + (this->width / 4) - (this->applet_mode_lbl->getWidth() / 2),
                this->y + this->height - (style->AppletFrame.footerHeight / 2),
                this->applet_mode_lbl->getWidth(),
                this->applet_mode_lbl->getHeight());
        }
        
        /* Time label. */
        this->time_lbl->invalidate(true);
        y_pos += this->y + 25 + this->time_lbl->getHeight();
        
        this->time_lbl->setBoundaries(
            this->x + this->width - (style->AppletFrame.separatorSpacing * 2) - this->time_lbl->getWidth(),
            y_pos,
            this->time_lbl->getWidth(),
            this->time_lbl->getHeight());
        
        /* Battery stats labels. */
        this->battery_icon->invalidate(true);
        this->battery_percentage->invalidate(true);
        y_pos += (20 + this->battery_icon->getHeight());
        
        this->battery_icon->setBoundaries(
            this->x + this->width - (style->AppletFrame.separatorSpacing * 2) - this->battery_percentage->getWidth() - 5 - this->battery_icon->getWidth(),
            y_pos,
            this->battery_icon->getWidth(),
            this->battery_icon->getHeight());
        
        this->battery_percentage->setBoundaries(
            this->x + this->width - (style->AppletFrame.separatorSpacing * 2) - this->battery_percentage->getWidth(),
            y_pos,
            this->battery_percentage->getWidth(),
            this->battery_percentage->getHeight());
        
        /* Network connection labels. */
        this->connection_icon->invalidate(true);
        this->connection_status_lbl->invalidate(true);
        y_pos += (20 + this->connection_icon->getHeight());
        
        this->connection_icon->setBoundaries(
            this->x + this->width - (style->AppletFrame.separatorSpacing * 2) - this->connection_status_lbl->getWidth() - 5 - this->connection_icon->getWidth(),
            y_pos,
            this->connection_icon->getWidth(),
            this->connection_icon->getHeight());
        
        this->connection_status_lbl->setBoundaries(
            this->x + this->width - (style->AppletFrame.separatorSpacing * 2) - this->connection_status_lbl->getWidth(),
            y_pos,
            this->connection_status_lbl->getWidth(),
            this->connection_status_lbl->getHeight());
    }
}
