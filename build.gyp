{
  'includes':[
  ],

  'variables': {
    'project_path':'.',
  },

  'target_defaults': {
    'cflags':['-g', '-O2', '-Wall', '-fPIC'],
    'defines':[],
    'include_dirs':[
    ],
  },

  'targets': [
    {
      'target_name' : 'leveldb_ex',
      'type' : 'executable',
      'include_dirs':[
        '<(project_path)/src',
        '<(project_path)/third_party/leveldb-1.13.0/include/',
      ],
      'link_settings': {
        'libraries' : [
          '-lboost_thread',
          '-lboost_system',
          '-lgflags',
          '-lglog',
          '-L<(project_path)/third_party/leveldb-1.13.0/',
          '-lleveldb',
        ]
      },
      'defines' : [],
      'dependencies' : [
      ],
      'sources' : [
        '<(project_path)/src/main.cc',
        '<(project_path)/src/net/connection.cc',
        '<(project_path)/src/net/packet.cc',
        '<(project_path)/src/net/request_handler.cc',
        '<(project_path)/src/net/request_parser.cc',
        '<(project_path)/src/net/server.cc',
        '<(project_path)/src/util/daemonize.cc',
        '<(project_path)/src/util/config.cc',
        '<(project_path)/src/storage/storage.cc',
      ],
    },
  ],
}

