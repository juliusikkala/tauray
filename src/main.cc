#include "tauray.hh"
#include <iostream>

int main(int, char** argv) try
{
    std::ios_base::sync_with_stdio(false);

    tr::options opt;
    tr::parse_command_line_options(argv, opt);

    bool worker = opt.worker;
    int port = opt.port;

    do
    {
        if(worker)
        {
            // TODO: Wait for connection and read options.
            //deserialize(opt, options_packet.data(), options_packet.size());
            opt.worker = worker;
        }
        else
        {
            std::vector<uint8_t> options_packet = serialize(opt);
            // TODO: try to connect to all workers listed in opt.connect.
            // TODO: Send options to workers
        }

        // Initialize log timer.
        tr::get_initial_time();
        std::optional<std::ofstream> timing_output_file;

        if(opt.silent)
        {
            tr::enabled_log_types[(uint32_t)tr::log_type::GENERAL] = false;
            tr::enabled_log_types[(uint32_t)tr::log_type::WARNING] = false;
        }

        if(opt.timing_output.size() != 0)
        {
            timing_output_file.emplace(opt.timing_output, std::ios::binary|std::ios::trunc);
            tr::log_output_streams[(uint32_t)tr::log_type::TIMING] = &timing_output_file.value();
        }

        std::unique_ptr<tr::context> ctx(tr::create_context(opt));

        tr::scene_data sd;
        if(worker)
        {
            // TODO: Send list of local devices to capitalist
            // TODO: Wait for capitalist to send list of all remote devices and
            // our device indices.
            // TODO: Add remote devices to context

            ctx->assign_network_ids();

            // TODO: implement load_network_scenes, should sync with
            // load_scenes.
            //sd = tr::load_network_scenes(*ctx, opt);
        }
        else
        {
            // TODO: Wait for workers to send device lists
            // TODO: Assign unique indices to all devices
            // TODO: Add remote devices to context

            size_t base_index = ctx->get_devices().size();

            ctx->get_network_devices().push_back({base_index++, 0, 1});
            ctx->get_network_devices().push_back({base_index++, 1, 1});

            ctx->assign_network_ids();
            // TODO: Send all device infos to all workers

            // TODO: Make load_scenes send each scene to all clients.
            sd = tr::load_scenes(*ctx, opt);
        }

        tr::run(*ctx, sd, opt);

        // TODO: Detect if quit by disconnect or ctrl-C. Disconnect should
        // continue looping if worker.
        break;
    }
    while(worker);

    return 0;
}
catch (std::runtime_error& e)
{
    // Can't use TR_ERR here, because the logger may not yet be initialized or
    // it's output file may already be closed.
    if (strlen(e.what())) std::cerr << e.what() << "\n" << std::endl;
    return 1;
}
